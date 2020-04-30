#pragma once

#include <atomic>
#include <vector>
#include <mutex>
#include <condition_variable>

#include <stdio.h>

namespace queue {

  /*
   * 一写多读 循环队列
   */
  class SwMrQueue {
  public:
    SwMrQueue(size_t capacity) :
      items_(capacity + 1),
      capacity_(capacity + 1),
      head_(0),
      tail_(0)
    {}

    bool enqueue(char c) {
      size_t tail = tail_.load(std::memory_order_relaxed);
      size_t head = head_.load(std::memory_order_relaxed);

      // 留一个条目不存储数据，用来区隔head和tail
      if ((tail + 1) % capacity_ == head)
        return false;

      items_[tail] = c;

      tail_.store((tail + 1) % capacity_, std::memory_order_release);

      return true;
    }

    bool dequeue(char &c) {
      while (true) {
        size_t head = head_.load(std::memory_order_relaxed);
        size_t tail = tail_.load(std::memory_order_relaxed);

        if (head == tail)
          return false;

        c = items_[head];

        if (head_.compare_exchange_strong(head, (head + 1) % capacity_, std::memory_order_release))
          return true;
      }
    }

  private:
    std::vector<char> items_;
    const size_t capacity_;
    std::atomic<size_t> head_;
    std::atomic<size_t> tail_;
  };

  /*
   * 多写多读 循环队列
   */
  class MwMrQueue {
  public:
    MwMrQueue(size_t capacity) :
      items_(capacity + 1),
      capacity_(capacity + 1),
      head_(0),
      tail_(0)
    {}

    bool full() {
      return (tail_ + 1) % capacity_ == head_;
    }

    void enqueue(char c) {
      std::unique_lock<std::mutex> lck(w_mutex_);
      w_cond_.wait(lck, [&]() { return !full(); });
      items_[tail_] = c;
      std::atomic_thread_fence(std::memory_order_release);
      tail_ = (tail_ + 1) % capacity_;
      r_cond_.notify_one();
    }

    bool empty() {
      return head_ == tail_;
    }

    char dequeue() {
      std::unique_lock<std::mutex> lck(r_mutex_);
      r_cond_.wait(lck, [&]() { return !empty(); });
      char c = items_[head_];
      std::atomic_thread_fence(std::memory_order_release);
      head_ = (head_ + 1) % capacity_;
      w_cond_.notify_one();
      return c;
    }

  private:
    std::vector<char> items_;
    const size_t capacity_;
    size_t head_;
    size_t tail_;
    std::mutex r_mutex_;
    std::mutex w_mutex_;
    std::condition_variable r_cond_;
    std::condition_variable w_cond_;
  };

  void test()
  {
    {
      SwMrQueue queue(3);

      queue.enqueue('a');
      queue.enqueue('b');
      queue.enqueue('c');

      char c;
      while (queue.dequeue(c)) {
        printf("ele: %c\n", c);
      }
    }

    {
      MwMrQueue queue(3);

      queue.enqueue('1');
      queue.enqueue('2');
      queue.enqueue('3');

      while (true) {
        printf("ele: %c\n", queue.dequeue());
      }
    }
  }

}