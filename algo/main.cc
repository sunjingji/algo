#include <assert.h>
#include <iostream>

template <class T>
void swap(T &a, T &b) {
  if (&a != &b) {
    T temp = a;
    a = b;
    b = temp;
  }
}

// 算法思想
#include "algorithm.h"

// 递归算法
#include "recursion.h"

// 动态规划
#include "dynamic_planning.h"

// 线性数据结构
#include "array.h"
#include "list.h"
#include "stack.h"
#include "queue.h"

// 排序
#include "sort.h"

// 查找：二分，跳表，哈希
#include "bsearch.h"
#include "skiplist.h"
#include "hash.h"

// 树：二叉树，多路查找树，堆
#include "binary_tree.h"
#include "btree.h"
#include "heap.h"

// 图
#include "graph.h"

// 字符串匹配：BF,RK,BM,KMP,Trie,AC
#include "match.h"

// 工具库
#include "message_buffer_queue.h"

int main(int argc, char **argv)
{
  //array::test();
  //list::test();
  //stack::test();
  //queue::test();

  //sort::test();

  //binary_search::test();
  //skiplist::test();
  //hash::test();

  //btree:test();
  //heap::test();

  //bm::test();
  //kmp::test();
  //trie::test();
  //ac::test();

  //recursion::test();
  //test();

  //package();
  //lcss("hish", "fish");

  getchar();
}