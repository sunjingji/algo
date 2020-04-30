#pragma once

#include <stdio.h>

/*
 * 二分查找
 *
 * 二分查找的典型变型：
 * 1)查找第一个值等于给定值的元素
 * 2)查找最后一个值等于给定值的元素
 * 3)查找第一个大于等于给定值的元素
 * 4)查找最后一个小于等于给定值的元素
 */
namespace binary_search {

  int bsearch(int a[], int n, int value)
  {
    int hi = n - 1;
    int lo = 0;
    while (lo <= hi) {
      int mid = lo + ((hi - lo) >> 1);
      if (value == a[mid])
        return mid;
      if (value < a[mid])
        hi = mid - 1;
      else
        lo = mid + 1;
    }

    return -1;
  }

  int bsearch_first_eq(int a[], int n, int value)
  {
    int hi = n - 1;
    int lo = 0;
    while (lo <= hi) {
      int mid = lo + ((hi - lo) >> 1);
      if (value == a[mid]) {
        if (mid == lo || a[mid - 1] != value) return mid;
        else hi = mid - 1;
      }
      else if (value < a[mid]) {
        hi = mid - 1;
      }
      else {
        lo = mid + 1;
      }
    }

    return -1;
  }

  int bsearch_last_eq(int a[], int n, int value)
  {
    int hi = n - 1;
    int lo = 0;
    while (lo <= hi) {
      int mid = lo + ((hi - lo) >> 1);
      if (value == a[mid]) {
        if (mid == hi || a[mid + 1] != value) return mid;
        else lo = lo + 1;
      }
      else if (value < a[mid]) {
        hi = mid - 1;
      }
      else {
        lo = mid + 1;
      }
    }

    return -1;
  }

  int bsearch_first_ge(int a[], int n, int value)
  {
    int hi = n - 1;
    int lo = 0;
    while (lo <= hi) {
      int mid = lo + ((hi - lo) >> 1);
      if (value <= a[mid]) {
        if (mid == lo || a[mid - 1] < value) return mid;
        else hi = mid - 1;
      }
      else {
        lo = mid + 1;
      }
    }

    return -1;
  }

  int bsearch_last_le(int a[], int n, int value)
  {
    int hi = n - 1;
    int lo = 0;
    while (lo <= hi) {
      int mid = lo + ((hi - lo) >> 1);
      if (value >= a[mid]) {
        if (mid == hi || a[mid + 1] > value) return mid;
        else lo = mid + 1;
      }
      else if (value < a[mid]) {
        hi = mid - 1;
      }
    }

    return -1;
  }

  void test()
  {

    {
      int a[] = { 1,2,3,5,7,8,9,11 };
      int n = sizeof(a) / sizeof(int);
      printf("\n-----------------bsearch-------------------\n");
      for (int i = 0; i < n; ++i) {
        printf("%d at %d\n", a[i], bsearch(a, n, a[i]));
      }
    }

    {
      int a[] = { 1,2,3,5,5,5,7,8,9,11 };
      int n = sizeof(a) / sizeof(int);

      printf("\n-----------------bsearch_first_eq-------------------\n");
      for (int i = 0; i < n; ++i) {
        printf("%d at %d\n", a[i], bsearch_first_eq(a, n, a[i]));
      }

      printf("\n-----------------bsearch_last_eq-------------------\n");
      for (int i = 0; i < n; ++i) {
        printf("%d at %d\n", a[i], bsearch_last_eq(a, n, a[i]));
      }

      printf("\n-----------------bsearch_first_ge-------------------\n");
      std::vector<int> items = { 1,2,3,4,5,6,7,8,9,10,11,12,13 };
      for (auto i : items) {
        int pos = bsearch_first_ge(a, n, i);
        if (pos >= 0)
          printf("first_ge %d is %d\n", i, a[pos]);
        else
          printf("first_ge %d is null\n", i);
      }

      printf("\n-----------------bsearch_first_le-------------------\n");
      for (auto i : items) {
        int pos = bsearch_last_le(a, n, i);
        if (pos >= 0)
          printf("last_le %d is %d\n", i, a[pos]);
        else
          printf("last_le %d is null\n", i);
      }
    }
  }

}