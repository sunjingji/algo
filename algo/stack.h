#pragma once

#include <stdio.h>
#include <stdlib.h>

namespace stack {

  /**
   * ��������ʵ�ֵ�˳��ջ
   */
  template<class T>
  class ArrayStack {
  private:
    T *array_;
    int size_;  // ջ������
    int count_; // ջ��Ԫ�صĸ���

  public:
    struct EmptyEx {};

    ArrayStack(int size)
      : size_(size), count_(0)
    {
      array_ = (T*)malloc(size * sizeof(T));
    }

    bool empty() {
      return count_ == 0;
    }

    bool full() {
      return count_ == size_;
    }

    void push(T c) {
      if (full()) throw EmptyEx();
      array_[count_++] = c;
    }

    T pop() {
      if (empty()) throw EmptyEx();
      return array_[--count_];
    }

    T top() {
      if (empty()) throw EmptyEx();
      return array_[count_ - 1];
    }

    void clean() {
      count_ = 0;
    }

    int count() {
      return count_;
    }

  };

  /*
   * ���ʽ��ֵ
   *
   * ʹ������ջ: ������ջ�������ջ
   *
   * ��������ɨ����ʽ���������֣���ֱ����ջ�����������������ջ����������Ƚ�;
   * ���������������ȼ����ߣ��Ͱ����������ջ��
   * ���������������ȼ�С�ڵ���ջ����������򵯳���Ӧ�Ĳ���������������м��㣬����������ջ�������Ƚϣ�
   */
  struct ExpressionEx {};

  int algo(char op, int x, int y)
  {
    switch (op) {
    case '+':
      return x + y;
    case '-':
      return x - y;
    case '*':
      return x * y;
    case '/':
      return x / y;
    default:
      throw ExpressionEx();
    }
  }
  int priority(char op)
  {
    switch (op) {
    case '+':
    case '-':
      return 1;
    case '*':
    case '/':
      return 2;
    default:
      throw ExpressionEx();
    }
  }
  int caculator(const char *s)
  {
    ArrayStack<char> opStack(16);
    ArrayStack<int> dataStack(16);

    char c;
    while ((c = *s))
    {
      if (c >= '0' && c <= '9') {  // ������
        int data = c - '0';
        int i;
        for (i = 1; i < sizeof(data); ++i) {
          if (!++s)
            break;
          c = *s;
          if (c >= '0' && c <= '9') {
            data = data * 10 + (c - '0');
            continue;
          }
          break;
        }
        if (i == sizeof(data)) {  // ������̫��
          throw ExpressionEx();
        }
        dataStack.push(data);
        continue;
      }
      else if (c == '+' || c == '-' || c == '*' || c == '/') {  // ������
        while (true) {
          if (opStack.empty() || opStack.top() == '(') {
            opStack.push(c);
            break;
          }
          if (priority(c) - priority(opStack.top()) <= 0) {
            int y = dataStack.pop();
            int x = dataStack.pop();
            int z = algo(opStack.pop(), x, y);
            dataStack.push(z);
          }
          else {
            opStack.push(c);
            break;
          }
        }
      }
      else if (c == '(') {  // ������
        opStack.push(c);
      }
      else if (c == ')') {  // ������
        while (!opStack.empty()) {
          if (opStack.top() == '(') {
            opStack.pop();
            break;
          }
          int y = dataStack.pop();
          int x = dataStack.pop();
          int z = algo(opStack.pop(), x, y);
          dataStack.push(z);
        }
      }
      else if (c == ' ') {  // �ո�
      }
      else {
        throw std::runtime_error("invalid expression");
      }

      // �ƶ�ָ��
      ++s;
    }

    while (!opStack.empty()) {
      int y = dataStack.pop();
      int x = dataStack.pop();
      int z = algo(opStack.pop(), x, y);
      dataStack.push(z);
    }

    int ret = 0;
    if (!dataStack.empty())
      ret = dataStack.pop();

    return ret;
  }

  /*
   * ģ�������ǰ������
   */
  class Explorer {
  private:
    ArrayStack<int> forward_;
    ArrayStack<int> backward_;
    int current_page_;

  public:
    Explorer(int n)
      : forward_(n), backward_(n), current_page_(0)
    {}

    void visitNewPage(int page) {
      if (!backward_.empty()) {  // ���ڻ��˳���, ������ǰҳ��
        forward_.push(current_page_);
      }
      backward_.clean();  // ��ջ���ҳ���������Է�����ʷ
      forward_.push(page);
      current_page_ = page;
      printf("++> page_%02d\n", page);
    }

    void forward() {
      if (backward_.empty())
        return;

      int page = backward_.pop();
      if (page == current_page_) { // ������ǰҳ
        forward_.push(page);
        page = backward_.pop();
      }

      forward_.push(page);
      current_page_ = page;
      printf("--> page_%02d\n", page);
    }

    void back() {
      int n = forward_.count();
      if (n == 0 ||  // ������ʷΪ��
        (n == 1 && backward_.empty()))  // ������ʷֻ��һ��ҳ��    
        return;

      int page = forward_.pop();
      if (page == current_page_) { // ������ǰҳ
        backward_.push(page);
        page = forward_.pop();
      }
      backward_.push(page);
      current_page_ = page;
      printf("<-- page_%02d\n", page);
    }
  };


  void test()
  {
    const char *s[] = {
      "34 + 13*9 + 44 - 12/3",
      "(4*(12+8)/2 - 10)",
      "10*10/(1) - 50 + 49"
    };

    for (int i = 0; i < sizeof(s) / sizeof(const char*); ++i) {
      printf("%s = %d\n", s[i], caculator(s[i]));
    }

    printf("\n---------monitor explorer----------\n");
    Explorer explorer(100);
    explorer.visitNewPage(1);
    explorer.visitNewPage(2);
    explorer.visitNewPage(3);
    explorer.visitNewPage(4);
    explorer.back();
    explorer.back();
    explorer.visitNewPage(5);
    explorer.back();
    explorer.forward();
  }

}