#include <vector>
#include <stdio.h>

/**
 * 调试vector存储空间扩容行为
 */
namespace array {
  void test() {
    std::vector<int> v;
    int *p = v.data();
    for (int i = 0; i < 10000; i++) {
      v.push_back(i);
      if (p != v.data()) {
        p = v.data();
        printf("realloc vec size %d\n", v.size());
      }
    }
  }
}