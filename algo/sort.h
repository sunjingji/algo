#pragma once

#include "stdlib.h"
#include "stdio.h"

/*
 *  考察排序算法的几个维度：时间复杂度，空间复杂度(是否原地排序)，稳定性;
 */

namespace sort {

  /*
   * 归并排序, 由下到上，线处理子分区，再合并得到上级分区的解
   */
  void mergeSort(int data[], int n)
  {
    if (n == 1)
      return;

    int mid = n / 2;
    mergeSort(data, mid);
    mergeSort(data + mid, n - mid);

    int *temp = (int*)malloc(sizeof(int)*n);
    int i = 0;
    int j = 0;
    int k = mid;
    for (; j < mid && k < n; ++i) {
      if (data[j] <= data[k]) {
        temp[i] = data[j++];
      }
      else {
        temp[i] = data[k++];
      }
    }

    for (; j < mid; ++i, ++j) {
      temp[i] = data[j];
    }
    for (; k < n; ++i, ++k) {
      temp[i] = data[k];
    }

    for (i = 0; i < n; ++i) {
      data[i] = temp[i];
    }

    free(temp);
  }

  /*
   * 快速排序，由上到下，先分区，再处理子分区
   */
  void qSort(int data[], int low, int high)
  {
    if (high <= low)
      return;

    // 选择中枢，优化：改为三数取中
    int mid = low + (high - low) / 2;
    int pivot = data[mid];

    // 分区
    swap(data[mid], data[high]);
    int i = low;
    for (int j = low; j < high; ++j) {
      if (data[j] < pivot) {  // Notes：右侧子序列>=pivot
        swap(data[j], data[i]);
        ++i;
      }
    }
    swap(data[i], data[high]);

    // 排序子分区
    qSort(data, low, i);
    qSort(data, i + 1, high);  // Notes: 必须排除data[i]，即pivot；否则，如果序列中有重复元素，可能无限递归；考虑 {5,5}
  }

  /*
   * 如何在O(n)内查找第k大元素
   */
  void findLargestK(int data[], int low, int high, int k)
  {
    if (high <= low)
      return;

    // 选择中枢，优化：改为三数取中
    int mid = low + (high - low) / 2;
    int pivot = data[mid];

    // 分区
    swap(data[mid], data[high]);
    int i = low;
    for (int j = low; j < high; ++j) {
      if (data[j] < pivot) {  // Notes：右侧子序列>=pivot
        swap(data[j], data[i]);
        ++i;
      }
    }
    swap(data[i], data[high]);

    // 找到第k大元素
    if (i + 1 == k)
      return;

    // 继续查找
    if (i + 1 > k)  // 第k大元素在左半分区
      findLargestK(data, low, i - 1, k);
    else  // 第k大元素在右半分区
      findLargestK(data, i + 1, high, k - i - 1);
  }

  /*
   * 桶排序
   *
   * 如果要排序的元素有n个，把它们均匀得划分到m个桶中，对每个桶内的k(n/m)个元素进行排序，然后依次遍历m个桶，就得到了排序后的数据；
   *
   * 桶排序的适用场景：
   * 1)首先，数据要能很容易划分成m个桶，并且桶与桶之间有着天然的大小关系。
   * 2)其次，数据在各个桶之间的分布是均匀的。
   *
   * 时间复杂度O(m*klogk)=O(nlogk)，当桶的个数m接近n时，logk就是一个非常小的常量，O(nlogk)才会接近于O(n);
   */

   /*
    * 计数排序
    *
    * 当每个桶内只有一个数值的数据时，就退化成了计数排序。
    *
    * 计数排序的适用场景：
    * 1)数据范围不大的场景。
    * 2)只能排序非负整数(考虑数组下标)。对于带负数，带小数点的数据类型，需要先转换为非负整数，才能使用计数排序。
    */

    /*
     * 基数排序
     *
     * 使用稳定排序算法，从最低位到最高位，依次进行排序。
     *
     * 适用场景：
     * 1)当可以把排序数据划分割出独立的“位”，而且位之间有递进关系，如果a数据的高位比b数据的高位大时，剩下的数据就不用比较了。
     * 2)每一位的数据范围不能太大，要可以使用线性排序算法进行排序，否则，基数排序就做不到O(n)了。
     */

  void test()
  {
    {
      printf("\n---------------mergeSort-------------------\n");

      int data[] = { 11,8,3,9,7,1,2,5 };
      int n = sizeof(data) / sizeof(int);
      mergeSort(data, n);
      for (int i = 0; i < n; ++i)
        printf("%d ", data[i]);
    }

    {
      printf("\n-----------------qSort---------------------\n");

      int data[] = { 11,8,3,9,7,1,2,5,5 };
      int n = sizeof(data) / sizeof(int);
      qSort(data, 0, n - 1);
      for (int i = 0; i < n; ++i)
        printf("%d ", data[i]);
    }

    {
      printf("\n-----------------findLargestK---------------------\n");

      int data[] = { 11,8,3,9,7,1,2,5 };
      int n = sizeof(data) / sizeof(int);
      for (int i = 1; i <= n; ++i) {
        findLargestK(data, 0, n - 1, i);
        printf("findLargestK(%d): %d\n", i, data[i - 1]);
      }
    }
  }

}