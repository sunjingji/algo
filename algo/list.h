#pragma once

#include <stdio.h>

namespace list {

  /*
   * 带头链表
   *
   * 哨兵节点,不存储数据
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
   * 单链表反转
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
   * 求链表的中间节点
   *
   * 使用快慢指针，慢指针每次前进一步，快指针每次前进二步
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
   * 链表中环的检测
   *
   * 假设环的长度为n, 那么经过n步后, 快节点正好多饶一圈，两个节点相遇;
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
   * 删除链表倒数第N个节点
   * 1)先求链表的长度k;
   * 2)然后根据删除元素的位置, 从链表头或中间节点(慢指针)开始遍历定位节点，并删除;
   */
  void deleteNodeAtReverseIndex(List &list, int n)
  {
    // 求链表长度, 确定删除元素的位置
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

    // 确定遍历的起点和步数
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

    // 定位和删除元素
    while (step-- > 0) {
      node = node->next;
    }
    deleteNode(list, node);
  }

  /*
   * 检测链表是否具有回文特征
   *
   * 使用快慢指针, 找到中点, 同时反转链表，检查前后两个子链是否相等
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
    if (right == nullptr) { // 链表只有1个节点
      return true;
    }
    while (left && (left->data == right->data)) {
      right = right->next;
      left = left->next;
    }

    bool has_plalindrome = left ? false : true;

    // 复原链表
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
   * 求滑动窗口内的最大值
   *
   * 解法：使用单调队列。
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

    // 求链表中间节点
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

    // 删除倒数第N个元素
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

    // 测试有环节点,  list: 1->2->3->4->5
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