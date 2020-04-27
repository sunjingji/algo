#pragma once

#include <stdlib.h>
#include <string.h>
#include <atomic>
#include <list>
#include <mutex>

class MessageBufferQueue;

/*
 * ������
 * �̶����ȣ�֧��һд�����
 * ������������WriteOpen״̬ʱ��Writer�����д�����ݣ����򣬱�ʾ�������رգ�Writer����д�����ݡ�
 */
class MessageBuffer {
public:
  MessageBuffer(size_t buffer_size)
    : buffer_size_(buffer_size)
  {
    data_ = (char*)malloc(buffer_size);
    reopen(0);
  }

  ~MessageBuffer() {
    free(data_);
  }

  int64_t id() const {
    return id_;
  }

  bool isOpen() const {
    return is_open_;
  }

  // �����ݣ������������ֽ��������ؿ��ֽ���������ָ��
  const char *read(size_t consumedBytes, size_t &readableBytes) {
    size_t size = size_.load(std::memory_order_acquire);
    if (consumedBytes < size) {
      readableBytes = size - consumedBytes;
      return data_ + consumedBytes;
    }
    readableBytes = 0;
    return nullptr;
  }

protected:
  friend class MessageBufferQueue;
  
  bool write(const char *data, size_t n) {
    if (is_open_) {
      size_t size = size_.load(std::memory_order_acquire);
      size_t free = buffer_size_ - size;
      if (n <= free) {
        memcpy(data_ + size, data, n);
        size_.store(size + n, std::memory_order_release);
        return true;
      }
      is_open_ = false;  // ��д�����ر�д
    }
    return false;
  }

  void reopen(int64_t id) {
    id_ = id;
    size_ = 0;
    is_open_ = true;
  }

  void close() {
    is_open_ = false;
  }

private:
  size_t buffer_size_;
  int64_t id_;
  bool is_open_;
  char *data_;
  std::atomic<size_t> size_;
};

/*
 * ����������
 *
 * �����ߴ���һ��MessageBuffer�������������β��Ȼ�����д�����������ֱ��д�����رջ�������Ȼ�������
 * β�����µ�MessageBufer������д���ݣ�����������
 * 
 * �����������MessageBuffer�������߶��ǿɼ��ģ����ԣ����ܻ���������ߺ�������ͬʱ����һ��MessageBuffer
 * ���������������������Case��MessageBuffer�����Ϊ֧�ֲ�����д(һ�������ߣ����������)��
 * 
 * ͨ��queue_min��queue_max�����ƶ��еĴ�С��ʵ��MessageBuffer���ã��Ͷ��г����Ե�ƽ�⡣
 * ����Ҫһ���µ�MessageBufferʱ��
 *  - �������size<queue_min���򴴽��µ�MessageBuffer
 *  - �������size>queue_min���������ͷ����MessageBuffer
 *    ���û����������ʹ�ã���������Ӳ����ã�������ڱ�������ʹ�ã��򴴽��µ�MessageBuffer
 * ������size=queue_maxʱ�����ٴ����µ�MessageBuffer��Ҳ����˵����������߶����ٶ�̫������һ��
 * MessageBuffer�����ݻ�û�ж��꣬��ô���޷���д�������ݡ�
 * ���磬buffer_size=16M��queue_min=4��queue_max=8������б�֤�������4*16M=64M�����ݣ���������
 * 8*16M=128M�����ݡ�
 */
class MessageBufferQueue {
public:
  typedef std::shared_ptr<MessageBuffer> Buffer;

  MessageBufferQueue(size_t buffer_size, size_t queue_min, size_t queue_max)
    : buffer_size_(buffer_size), 
    queue_min_(queue_min),
    queue_max_(queue_max)
  {
    current_buffer_.reset(new MessageBuffer(buffer_size));
    current_buffer_->reopen(1);
    std::unique_lock<std::mutex> lck(mtx_);
    queue_.push_back(current_buffer_);
  }

  ~MessageBufferQueue()
  {
    std::unique_lock<std::mutex> lck(mtx_);
    queue_.clear();
  }

  Buffer fetch(int64_t lastBufferId)
  {
    std::unique_lock<std::mutex> lck(mtx_);
    for (auto it : queue_) {
      if (it->id() > lastBufferId)
        return it;
    }
    return nullptr;
  }

  bool publish(const char *data, size_t len)
  {
    if (len > buffer_size_)  // ��Ϣ���Ȳ��ܳ����������Ĵ�С
      return false;
    if (current_buffer_ && current_buffer_->write(data, len))  // �����ǰ�������ռ��㹻��д��
      return true;
    if (changeCurrentBuffer())  // �����ǰ�ռ䲻�㣬����������
      return current_buffer_->write(data, len);
    return false;
  }

private:
  bool changeCurrentBuffer()
  {
    int64_t last_id = current_buffer_->id();
    current_buffer_->close();
    current_buffer_.reset();
    if (queue_.size() > queue_min_) {  // ���ҿ��л�����          
      std::unique_lock<std::mutex> lck(mtx_);
      if (queue_.front().unique()) {
        current_buffer_ = queue_.front();
        queue_.pop_front();
      }
    }
    if (current_buffer_ == nullptr) {  // �����»�����
      if (queue_.size() < queue_max_) {
        current_buffer_.reset(new MessageBuffer(buffer_size_));
      }
    }
    if (current_buffer_) {  // ��ʼ�������������뵽����ĩβ
      current_buffer_->reopen(last_id + 1);
      std::unique_lock<std::mutex> lck(mtx_);      
      queue_.push_back(current_buffer_);
      return true;
    }
    return false;
  }

private: 
  size_t buffer_size_;
  size_t queue_min_;
  size_t queue_max_;
  std::list<Buffer> queue_;
  std::mutex mtx_;
  Buffer current_buffer_;
};