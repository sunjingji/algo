#pragma once

#include <stdio.h>
#include <map>

namespace recursion {

   /*
    * ¥������
    * ������n��̨�ף���һ�ο��Կ�1��̨�׻�2��̨�ף�����������n��̨���ж������߷���
    */
  int stairs(int n)
  {
    // ��һ�µݹ���������Է�ֹ�ظ�����
    std::map<int, int> result;

    if (n == 1) {
      return 1;
    }
    else if (n == 2) {
      return 2;
    }
    else {
      auto it = result.find(n);
      if (it != result.end()) {
        return it->second;
      }
      else {
        int ret = stairs(n - 1) + stairs(n - 2);
        result[n] = ret;
        return ret;
      }
    }
  }

  /*
   * ����������⣬���͵Ļ��ݵݹ�����
   */
  void printPermutations(int data[], int n, int k)
  {
    // ���������һ�������⣬�͵������һ�ֽ�
    if (k == 1) {
      for (int i = 0; i < n; ++i)
        printf("%d ", data[i]);
      printf("\n");
      return;
    }

    for (int i = 0; i < k; ++i) {
      swap(data[i], data[k - 1]);
      printPermutations(data, n, k - 1);
      swap(data[i], data[k - 1]);
    }
  }

  /*
   * �˻ʺ����⣬���ͻ��ݵݹ�����
   */
  int gQueue[8];  // 8�ʺ�ֱ����ڵ��� 
  int gCount = 0;  // �ⷨ����

  bool isInvalidPos(int line, int col)
  {
    for (int i = 0; i < line; ++i) {
      if (gQueue[i] == col)  // ÿһ�����һ���ʺ�
        return false;
      if (abs(i - line) == abs(gQueue[i] - col)) {  // ���Խ���
        return false;
      }
    }

    return true;
  }

  void printQueue(int k)
  {
    for (int i = 0; i < 8; i++) {
      if (isInvalidPos(k, i)) {
        gQueue[k] = i;  // ���õ�k���ʺ�
        if (k == 7) {  // ȷ�������һ���ʺ��λ�ã�����ݷý��
          ++gCount;
          printf("\n------------------%d---------------\n", gCount);
          for (int j = 0; j < 8; ++j) {
            for (int s = 0; s < gQueue[j]; ++s) printf("#");
            printf("Q\n");
          }
        }
        else {
          printQueue(k + 1);
        }
      }
    }
  }

  /*
   * ȡ�����⣺��n������(ÿ�����Ų�ͬ)������ȡm��(���Ż�)���ж�����ȡ����
   *
   * �ȼ���������⣬f(n,m) = (n*m)/2
   *
   * ����n��������һ��������һ��ȡ��Ҫô����������Ҫô���������������ⱻ�ֽ�Ϊ������ģ��С�����⣺
   *    ����������f(n) = f(n-1,m-1)
   *    ����������f(n-1,m)
   */
  int takeBall(int ball[], int n, int m)
  {
    if (m == 0)
      return 1;
    if (m == n)
      return 1;
    if (m > n)
      return 0;

    return takeBall(ball, n - 1, m - 1) + takeBall(ball, n - 1, m);
  }

  void test()
  {
    {
      printf("\n--------- stairs ----------\n");
      for (int i = 1; i < 10; i++)
        printf("stairs(%d) -> %d\n", i, stairs(i));
    }

    {
      printf("\n------ permutations -------\n");
      int data[] = { 1,2,3 };
      int n = sizeof(data) / sizeof(int);
      printPermutations(data, n, n);
    }

    {
      printf("\n--------- queues ----------\n");
      printQueue(0);
    }

    {
      printf("\n--------- takeBall ----------\n");
      int ball[] = { 1,2,3,4 };
      int n = sizeof(ball) / sizeof(int);
      int m = 2;
      printf("ball n=%d, m=%d, method=%d\n", n, m, takeBall(ball, n, m));
    }
  }

}