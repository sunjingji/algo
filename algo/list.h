#pragma once

#include <stdio.h>

namespace list {

  /*
   * ��ͷ����
   *
   * �ڱ��ڵ�,���洢����
   */
  struct LinkNode {
    int data;
    LinkNode *next;
  };

  typedef LinkNode List;

  void initialize(List &list)
  {
    list.data = 0;
    list.next = nullptr;
  }

  bool empty(const List &list)
  {
    return list.next == nullptr;
  }

  void insertNode(List &list, int data)
  {
    auto *node = new LinkNode();
    node->data = data;
    node->next = list.next;
    list.next = node;
  }

  bool deleteNode(List &list, LinkNode *node)
  {
    if (!node)
      return false;

    auto *prev = &list;
    auto *cur = list.next;
    while (cur && cur != node) {
      prev = cur;
      cur = cur->next;
    }
    if (cur == node) {
      prev->next = cur->next;
      delete node;
      return true;
    }

    return false;
  }

  /*
   * ������ת
   */
  void reverse(List &list)
  {
    LinkNode *head = nullptr;
    LinkNode *node = list.next;
    while (node) {
      auto *next = node->next;
      node->next = head;
      head = node;
      node = next;
    }
    list.next = head;
  }

  /*
   * ��������м�ڵ�
   *
   * ʹ�ÿ���ָ�룬��ָ��ÿ��ǰ��һ������ָ��ÿ��ǰ������
   */
  LinkNode* findMiddle(List &list)
  {
    if (empty(list))
      return nullptr;

    LinkNode *slow = &list;
    LinkNode *fast = &list;
    while (fast && fast->next) {
      slow = slow->next;
      fast = fast->next->next;
    }

    return slow;
  }

  /*
   * �����л��ļ��
   *
   * ���軷�ĳ���Ϊn, ��ô����n����, ��ڵ����ö���һȦ�������ڵ�����;
   */
  int hasCircle(List &list)
  {
    LinkNode *slow = &list;
    LinkNode *fast = &list;
    int n = 0;
    while (fast && fast->next) {
      ++n;
      slow = slow->next;
      fast = fast->next->next;
      if (slow == fast)
        return n;
    }

    return 0;
  }

  /*
   * ɾ����������N���ڵ�
   * 1)��������ĳ���k;
   * 2)Ȼ�����ɾ��Ԫ�ص�λ��, ������ͷ���м�ڵ�(��ָ��)��ʼ������λ�ڵ㣬��ɾ��;
   */
  void deleteNodeAtReverseIndex(List &list, int n)
  {
    // ��������, ȷ��ɾ��Ԫ�ص�λ��
    LinkNode *slow = &list;
    LinkNode *fast = &list;
    int cur_pos = 0;
    while (fast && fast->next) {
      slow = slow->next;
      fast = fast->next->next;
      ++cur_pos;
    }
    int len = fast ? cur_pos * 2 : cur_pos * 2 - 1;
    int delete_pos = len - n + 1;
    if (delete_pos <= 0)
      return;

    // ȷ�����������Ͳ���
    LinkNode *node = nullptr;
    int step = 0;
    if (delete_pos < cur_pos) {
      node = &list;
      step = delete_pos;
    }
    else {
      node = slow;
      step = delete_pos - cur_pos;
    }

    // ��λ��ɾ��Ԫ��
    while (step-- > 0) {
      node = node->next;
    }
    deleteNode(list, node);
  }

  /*
   * ��������Ƿ���л�������
   *
   * ʹ�ÿ���ָ��, �ҵ��е�, ͬʱ��ת�������ǰ�����������Ƿ����
   */
  bool hasPlalindrome(List &list)
  {
    if (empty(list))
      return false;

    LinkNode *head = nullptr;
    LinkNode *slow = &list;
    LinkNode *fast = &list;
    while (fast && fast->next) {
      slow = slow->next;
      fast = fast->next->next;

      auto *node = slow->next;
      slow->next = head;
      head = slow;
      slow->next = node;
    }

    LinkNode *left = fast ? head : head->next;
    LinkNode *right = slow->next;
    if (right == nullptr) { // ����ֻ��1���ڵ�
      return true;
    }
    while (left && (left->data == right->data)) {
      right = right->next;
      left = left->next;
    }

    bool has_plalindrome = left ? false : true;

    // ��ԭ����
    list.next = slow->next;
    while (head) {
      auto *node = head;
      head = node->next;
      node->next = list.next;
      list.next = node;
    }

    return has_plalindrome;
  }

  /*
   * �󻬶������ڵ����ֵ
   *
   * �ⷨ��ʹ�õ������С�
   */

  void print(List &list)
  {
    auto *node = list.next;
    while (node) {
      printf("%d ", node->data);
      node = node->next;
    }
    printf("\n");
  }

  void test() {

    // �������м�ڵ�
    {
      List list;
      initialize(list);

      insertNode(list, 1);
      insertNode(list, 2);
      insertNode(list, 3);
      insertNode(list, 4);
      insertNode(list, 5);

      printf("initial link: ");
      print(list);

      printf("reverse link: ");
      reverse(list);
      print(list);

      LinkNode *node;
      while ((node = findMiddle(list))) {
        printf("after delete mid[%d] link: ", node->data);
        deleteNode(list, node);
        print(list);
      }
    }

    // ɾ��������N��Ԫ��
    {
      printf("\n----------------------------------\n");

      List list;
      initialize(list);

      insertNode(list, 1);
      insertNode(list, 2);
      insertNode(list, 3);
      insertNode(list, 4);
      insertNode(list, 5);
      reverse(list);
      printf("initial link: ");
      print(list);

      deleteNodeAtReverseIndex(list, 10);
      printf("after delete reverse 10: ");
      print(list);
      deleteNodeAtReverseIndex(list, 3);
      printf("after delete reverse 3: ");
      print(list);
      deleteNodeAtReverseIndex(list, 4);
      printf("after delete reverse 4: ");
      print(list);
    }

    // �����л��ڵ�,  list: 1->2->3->4->5
    {
      printf("\n----------------------------------\n");

      List list;
      initialize(list);

      insertNode(list, 5);
      insertNode(list, 4);
      insertNode(list, 3);
      insertNode(list, 2);
      insertNode(list, 1);
      printf("initial link: ");
      print(list);

      auto *node = findMiddle(list);  // assert(node.data == 3)
      node->next = list.next;   // 3->1;
      printf("hasCircle: %d", hasCircle(list));  // asset(hasCircle(list) == 3);
    }

    {
      printf("\n----------------------------------\n");

      List list;
      initialize(list);

      insertNode(list, 5);
      insertNode(list, 4);
      insertNode(list, 3);
      insertNode(list, 4);
      insertNode(list, 5);
      printf("initial link: ");
      print(list);

      printf("hasPlalindrome: %d", hasPlalindrome(list));
    }
  }

}