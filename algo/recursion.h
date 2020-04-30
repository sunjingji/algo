#pragma once

#include <stdio.h>
#include <map>

namespace recursion {

   /*
    * 楼梯问题
    * 假如有n个台阶，你一次可以跨1个台阶或2个台阶，请问走完这n个台阶有多少中走法？
    */
  int stairs(int n)
  {
    // 画一下递归树，这可以防止重复计算
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
   * 排列组合问题，典型的回溯递归问题
   */
  void printPermutations(int data[], int n, int k)
  {
    // 处理完最低一级子问题，就到问题的一种解
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
   * 八皇后问题，典型回溯递归问题
   */
  int gQueue[8];  // 8皇后分别所在的列 
  int gCount = 0;  // 解法数量

  bool isInvalidPos(int line, int col)
  {
    for (int i = 0; i < line; ++i) {
      if (gQueue[i] == col)  // 每一列最多一个皇后
        return false;
      if (abs(i - line) == abs(gQueue[i] - col)) {  // 检查对角线
        return false;
      }
    }

    return true;
  }

  void printQueue(int k)
  {
    for (int i = 0; i < 8; i++) {
      if (isInvalidPos(k, i)) {
        gQueue[k] = i;  // 放置第k个皇后
        if (k == 7) {  // 确定了最后一个皇后的位置，输出拜访结果
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
   * 取球问题：在n个球中(每个球编号不同)，任意取m个(不放回)，有多少种取法。
   *
   * 等价于组合问题，f(n,m) = (n*m)/2
   *
   * 想象n个球当中有一个幸运球，一种取法要么包含幸运球，要么不包含幸运球，问题被分解为两个规模更小的问题：
   *    包含幸运球：f(n) = f(n-1,m-1)
   *    不含幸运球：f(n-1,m)
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