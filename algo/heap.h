#pragma once

#include <vector>
#include <assert.h>

/*
 * 堆
 * 
 * 父节点小于/大于叶子节点，没有排序二叉树要求严格。
 *
 * 插入/删除操作，保证堆是完全二叉树， 
 *   插入：最后一个叶子节点，再上翻调整顺序
 *   删除：堆顶元素，把最后叶子节点放置到堆顶，再下翻调整顺序 
 */
namespace heap {

  class Heap {
  public:
    struct EmptyEx {};

    Heap()
    {
      heap_.resize(1);
    }

    // 算法复杂度O(n)
    Heap(std::vector<int> &&data)
    {
      if (data.empty())
        return;

      // 第0个位置不存储数据
      data.push_back(std::move(data[0]));
      heap_ = std::move(data);

      // 建堆：从叶子节点开始，逐个元素上翻
      for (size_t p = heap_.size() - 1; p > 1; --p) {
        size_t pp = p >> 1;
        if (heap_[p] < heap_[pp])
          std::swap(heap_[p], heap_[pp]);
      }
    }

    bool empty() {
      return heap_.size() == 1;
    }

    size_t size() {
      return heap_.size() - 1;
    }

    int top() {
      if (empty())
        throw EmptyEx{};
      return heap_[0];
    }

    // 算法复杂度O(logn)
    void push(int data) {
      heap_.push_back(data);

      // 放到最后一个叶子节点，上翻
      size_t p = size();  // 要上翻的节点
      size_t pp;
      while ((pp = (p >> 1)) > 0 && heap_[p] < heap_[pp]) {
        std::swap(heap_[p], heap_[pp]);
        p = pp;
      }
    }

    // 算法复杂度O(logn)
    int pop() {
      if (empty())
        throw EmptyEx{};

      int ret = std::move(heap_[1]);

      // 把最右叶子节点交换到root
      heap_[1] = std::move(heap_[size()]);
      heap_.resize(heap_.size() - 1);

      // 下翻
      size_t pp = 1;  // 下翻节点    
      while (true) {
        size_t p = pp << 1;
        int npp = pp;  // 记录pp将要与哪个元素交换
        if (p <= size() && heap_[p] < heap_[npp])
          npp = p;
        if (++p <= size() && heap_[p] < heap_[npp])
          npp = p;
        if (npp == pp)  // 无需交换，下翻结束
          break;
        std::swap(heap_[pp], heap_[npp]);
        pp = npp;
      }

      return ret;
    }

  private:
    std::vector<int> heap_;
  };

  /*
   * 堆排序，
   *
   * 实现方法：建堆，交换堆顶和最后一个元素，下翻，实现原地排序
   *
   */

   /*
    * 堆的应用
    *
    * - 优先队列，用于实现定时器等，貌似redis的定时器使用的是4-heap
    *
    * - 求动态数据的TopK
    *   维护一个大小为K的小顶堆，如果元素M大于堆顶元素，则删除堆顶元素，把M插入到堆中，最后得到的堆就是TopK
    *   时间复杂度: O(nlogK)
    *
    * - 求动态数据的中位数
    *   1)大顶堆，维护小数；小顶堆，维护大数；两个堆的大小相等，或者大顶堆-小顶堆=1；
    *   2)插入数据X时，如果X小于大顶堆的堆顶元素，则插入大顶堆，否则插入小顶堆；
    *   3)每次插入数据后，重新计算两个堆的大小，如果不符合上面的要求，则在两个堆之间移动数据；
    *   4)大顶堆的堆顶元素即为中位数;
    *
    * - 求动态数据的99%百分位数
    *   1)与求中位数类似，大顶堆维护99%的数据，小顶堆维护1%的数据；
    */

  void test()
  {
    Heap heap({ 1,4,80,50,14,15 });
    //Heap heap;
    std::vector<int> data = { 1,4,80,50,14,15 };
    for (auto d : data) {
      heap.push(d);
    }

    for (int i = 0; i < 100; i++) {
      heap.push(std::rand());
    }

    int n = -1;
    while (!heap.empty()) {
      int k = heap.pop();
      assert(n <= k);
      n = k;
      printf("%d\n", k);
    }
  }

}