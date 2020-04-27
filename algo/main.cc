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

// �㷨˼��
#include "algorithm.h"

// �ݹ��㷨
#include "recursion.h"

// ��̬�滮
#include "dynamic_planning.h"

// �������ݽṹ
#include "array.h"
#include "list.h"
#include "stack.h"
#include "queue.h"

// ����
#include "sort.h"

// ���ң����֣�������ϣ
#include "bsearch.h"
#include "skiplist.h"
#include "hash.h"

// ��������������·����������
#include "binary_tree.h"
#include "btree.h"
#include "heap.h"

// ͼ
#include "graph.h"

// �ַ���ƥ�䣺BF,RK,BM,KMP,Trie,AC
#include "match.h"

// ���߿�
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