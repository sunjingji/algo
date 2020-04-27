#pragma once

#include <stdlib.h>
#include <string.h>
#include <atomic>
#include <list>
#include <mutex>

class MessageBufferQueue;

/*
 * 缓冲区
 * 固定长度，支持一写多读。
 * 当缓冲区处于WriteOpen状态时，Writer会持续写入数据；否则，表示缓冲区关闭，Writer不再写入数据。
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

  // 读数据，输入已消费字节数，返回可字节数和数据指针
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
      is_open_ = false;  // 已写满，关闭写
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
 * 缓冲区队列
 *
 * 生产者创建一个MessageBuffer，把它到放入队尾，然后持续写这个缓冲区，直到写满并关闭缓冲区。然后，再向队
 * 尾放置新的MessageBufer，继续写数据，依次往复。
 * 
 * 队列里的所有MessageBuffer对消费者都是可见的，所以，可能会出现生产者和消费者同时操作一个MessageBuffer
 * 的情况，在这里是正常的Case。MessageBuffer被设计为支持并发读写(一个生产者，多个消费者)。
 * 
 * 通过queue_min和queue_max来控制队列的大小，实现MessageBuffer复用，和队列持续性的平衡。
 * 当需要一个新的MessageBuffer时，
 *  - 如果队列size<queue_min，则创建新的MessageBuffer
 *  - 如果队列size>queue_min，则检查队列头部的MessageBuffer
 *    如果没有消费者在使用，则把它出队并复用；如果正在被消费者使用，则创建新的MessageBuffer
 * 当队列size=queue_max时，不再创建新的MessageBuffer。也就是说，如果消费者读的速度太慢，第一个
 * MessageBuffer的数据还没有读完，那么就无法再写入新数据。
 * 假如，buffer_size=16M，queue_min=4，queue_max=8，则队列保证保留最近4*16M=64M的数据，最多可容纳
 * 8*16M=128M的数据。
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
    if (len > buffer_size_)  // 消息长度不能超过缓冲区的大小
      return false;
    if (current_buffer_ && current_buffer_->write(data, len))  // 如果当前缓存区空间足够，写入
      return true;
    if (changeCurrentBuffer())  // 如果当前空间不足，更换缓冲区
      return current_buffer_->write(data, len);
    return false;
  }

private:
  bool changeCurrentBuffer()
  {
    int64_t last_id = current_buffer_->id();
    current_buffer_->close();
    current_buffer_.reset();
    if (queue_.size() > queue_min_) {  // 查找空闲缓冲区          
      std::unique_lock<std::mutex> lck(mtx_);
      if (queue_.front().unique()) {
        current_buffer_ = queue_.front();
        queue_.pop_front();
      }
    }
    if (current_buffer_ == nullptr) {  // 创建新缓冲区
      if (queue_.size() < queue_max_) {
        current_buffer_.reset(new MessageBuffer(buffer_size_));
      }
    }
    if (current_buffer_) {  // 初始化缓冲区，加入到队列末尾
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