#pragma once

#include <algorithm>
#include <vector>

// 小偷背包问题
void package()
{
  const int max_n = 3;
  const int max_w = 4;
  int value[] = { 3000,2000,1500 };
  int weight[] = { 4,3,1 };
  int max_value = 0;
  int select[max_n][max_w+1];

  for (int w = 1; w <= max_w; ++w) {
    select[0][w] = weight[0] <= w ? value[0] : 0;
  }
  for (int i = 1; i < max_n; ++i) {
    for (int w = 1; w <= max_w; ++w) {
      int maxv = select[i - 1][w];
      int left = w - weight[i];
      if (left >= 0) {
        int v = value[i] + select[i-1][left];
        if (v > maxv)
          maxv = v;
      }
      select[i][w] = maxv;
    }
  }

  for (int i = 0; i < max_n; ++i) {
    printf("%04d,%04d,%04d,%04d\n", select[i][1], select[i][2], select[i][3], select[i][4]);
  }
}

// 最长公共字串
// 每个单元格，代表两个字串的最长后缀字串长度
void lcss(const char *w1, const char *w2)
{
  int max = 0;

  int n1 = strlen(w1);
  int n2 = strlen(w2);

  int **m = new int*[n1];
  for (int i = 0; i < n1; ++i) {
    m[i] = new int[n2];
    for (int j = 0; j < n2; ++j)
      m[i][j] = 0;
  }
  for (int i = 0; i < n1; ++i) {
    if (w2[0] == w1[i])
      m[i][0] = 1;
  }
  for (int j = 0; j < n2; ++j) {
    if (w1[0] == w2[j])
      m[0][j] = 1;
  }

  for (int i = 1; i < n1; ++i) {
    for (int j = 1; j < n2; ++j) {
      if (w1[i] == w2[j]) {
        m[i][j] = m[i - 1][j - 1] + 1;
        if (m[i][j] > max)
          max = m[i][j];
      }
    }
  }

  printf("max: %d\n", max);
  printf("   ");
  for (int j = 0; j < n2; ++j) printf("%c  ", w2[j]);
  printf("\n");
  for (int i = 0; i < n1; ++i) {
    printf("%c  ", w1[i]);
    for (int j = 0; j < n2; ++j) {
      printf("%d  ", m[i][j]);
    }
    printf("\n");
  }
}