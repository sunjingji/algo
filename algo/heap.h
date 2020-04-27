#pragma once

#include <vector>
#include <assert.h>

/*
 * ��
 * 
 * ���ڵ�С��/����Ҷ�ӽڵ㣬û�����������Ҫ���ϸ�
 *
 * ����/ɾ����������֤������ȫ�������� 
 *   ���룺���һ��Ҷ�ӽڵ㣬���Ϸ�����˳��
 *   ɾ�����Ѷ�Ԫ�أ������Ҷ�ӽڵ���õ��Ѷ������·�����˳�� 
 */
namespace heap {

  class Heap {
  public:
    struct EmptyEx {};

    Heap()
    {
      heap_.resize(1);
    }

    // �㷨���Ӷ�O(n)
    Heap(std::vector<int> &&data)
    {
      if (data.empty())
        return;

      // ��0��λ�ò��洢����
      data.push_back(std::move(data[0]));
      heap_ = std::move(data);

      // ���ѣ���Ҷ�ӽڵ㿪ʼ�����Ԫ���Ϸ�
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

    // �㷨���Ӷ�O(logn)
    void push(int data) {
      heap_.push_back(data);

      // �ŵ����һ��Ҷ�ӽڵ㣬�Ϸ�
      size_t p = size();  // Ҫ�Ϸ��Ľڵ�
      size_t pp;
      while ((pp = (p >> 1)) > 0 && heap_[p] < heap_[pp]) {
        std::swap(heap_[p], heap_[pp]);
        p = pp;
      }
    }

    // �㷨���Ӷ�O(logn)
    int pop() {
      if (empty())
        throw EmptyEx{};

      int ret = std::move(heap_[1]);

      // ������Ҷ�ӽڵ㽻����root
      heap_[1] = std::move(heap_[size()]);
      heap_.resize(heap_.size() - 1);

      // �·�
      size_t pp = 1;  // �·��ڵ�    
      while (true) {
        size_t p = pp << 1;
        int npp = pp;  // ��¼pp��Ҫ���ĸ�Ԫ�ؽ���
        if (p <= size() && heap_[p] < heap_[npp])
          npp = p;
        if (++p <= size() && heap_[p] < heap_[npp])
          npp = p;
        if (npp == pp)  // ���轻�����·�����
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
   * ������
   *
   * ʵ�ַ��������ѣ������Ѷ������һ��Ԫ�أ��·���ʵ��ԭ������
   *
   */

   /*
    * �ѵ�Ӧ��
    *
    * - ���ȶ��У�����ʵ�ֶ�ʱ���ȣ�ò��redis�Ķ�ʱ��ʹ�õ���4-heap
    *
    * - ��̬���ݵ�TopK
    *   ά��һ����СΪK��С���ѣ����Ԫ��M���ڶѶ�Ԫ�أ���ɾ���Ѷ�Ԫ�أ���M���뵽���У����õ��ĶѾ���TopK
    *   ʱ�临�Ӷ�: O(nlogK)
    *
    * - ��̬���ݵ���λ��
    *   1)�󶥶ѣ�ά��С����С���ѣ�ά�������������ѵĴ�С��ȣ����ߴ󶥶�-С����=1��
    *   2)��������Xʱ�����XС�ڴ󶥶ѵĶѶ�Ԫ�أ������󶥶ѣ��������С���ѣ�
    *   3)ÿ�β������ݺ����¼��������ѵĴ�С����������������Ҫ������������֮���ƶ����ݣ�
    *   4)�󶥶ѵĶѶ�Ԫ�ؼ�Ϊ��λ��;
    *
    * - ��̬���ݵ�99%�ٷ�λ��
    *   1)������λ�����ƣ��󶥶�ά��99%�����ݣ�С����ά��1%�����ݣ�
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