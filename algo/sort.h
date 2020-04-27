#pragma once

#include "stdlib.h"
#include "stdio.h"

/*
 *  ���������㷨�ļ���ά�ȣ�ʱ�临�Ӷȣ��ռ临�Ӷ�(�Ƿ�ԭ������)���ȶ���;
 */

namespace sort {

  /*
   * �鲢����, ���µ��ϣ��ߴ����ӷ������ٺϲ��õ��ϼ������Ľ�
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
   * �����������ϵ��£��ȷ������ٴ����ӷ���
   */
  void qSort(int data[], int low, int high)
  {
    if (high <= low)
      return;

    // ѡ�����࣬�Ż�����Ϊ����ȡ��
    int mid = low + (high - low) / 2;
    int pivot = data[mid];

    // ����
    swap(data[mid], data[high]);
    int i = low;
    for (int j = low; j < high; ++j) {
      if (data[j] < pivot) {  // Notes���Ҳ�������>=pivot
        swap(data[j], data[i]);
        ++i;
      }
    }
    swap(data[i], data[high]);

    // �����ӷ���
    qSort(data, low, i);
    qSort(data, i + 1, high);  // Notes: �����ų�data[i]����pivot������������������ظ�Ԫ�أ��������޵ݹ飻���� {5,5}
  }

  /*
   * �����O(n)�ڲ��ҵ�k��Ԫ��
   */
  void findLargestK(int data[], int low, int high, int k)
  {
    if (high <= low)
      return;

    // ѡ�����࣬�Ż�����Ϊ����ȡ��
    int mid = low + (high - low) / 2;
    int pivot = data[mid];

    // ����
    swap(data[mid], data[high]);
    int i = low;
    for (int j = low; j < high; ++j) {
      if (data[j] < pivot) {  // Notes���Ҳ�������>=pivot
        swap(data[j], data[i]);
        ++i;
      }
    }
    swap(data[i], data[high]);

    // �ҵ���k��Ԫ��
    if (i + 1 == k)
      return;

    // ��������
    if (i + 1 > k)  // ��k��Ԫ����������
      findLargestK(data, low, i - 1, k);
    else  // ��k��Ԫ�����Ұ����
      findLargestK(data, i + 1, high, k - i - 1);
  }

  /*
   * Ͱ����
   *
   * ���Ҫ�����Ԫ����n���������Ǿ��ȵû��ֵ�m��Ͱ�У���ÿ��Ͱ�ڵ�k(n/m)��Ԫ�ؽ�������Ȼ�����α���m��Ͱ���͵õ������������ݣ�
   *
   * Ͱ��������ó�����
   * 1)���ȣ�����Ҫ�ܺ����׻��ֳ�m��Ͱ������Ͱ��Ͱ֮��������Ȼ�Ĵ�С��ϵ��
   * 2)��Σ������ڸ���Ͱ֮��ķֲ��Ǿ��ȵġ�
   *
   * ʱ�临�Ӷ�O(m*klogk)=O(nlogk)����Ͱ�ĸ���m�ӽ�nʱ��logk����һ���ǳ�С�ĳ�����O(nlogk)�Ż�ӽ���O(n);
   */

   /*
    * ��������
    *
    * ��ÿ��Ͱ��ֻ��һ����ֵ������ʱ�����˻����˼�������
    *
    * ������������ó�����
    * 1)���ݷ�Χ����ĳ�����
    * 2)ֻ������Ǹ�����(���������±�)�����ڴ���������С������������ͣ���Ҫ��ת��Ϊ�Ǹ�����������ʹ�ü�������
    */

    /*
     * ��������
     *
     * ʹ���ȶ������㷨�������λ�����λ�����ν�������
     *
     * ���ó�����
     * 1)�����԰��������ݻ��ָ�������ġ�λ��������λ֮���еݽ���ϵ�����a���ݵĸ�λ��b���ݵĸ�λ��ʱ��ʣ�µ����ݾͲ��ñȽ��ˡ�
     * 2)ÿһλ�����ݷ�Χ����̫��Ҫ����ʹ�����������㷨�������򣬷��򣬻��������������O(n)�ˡ�
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