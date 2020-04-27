#pragma once

#include <algorithm>
#include <memory>
#include <queue>
#include <map>

/*
 * ----------------------------------------------------
 * 单模式字符串匹配

 * 从朴素匹配算法BF开始，在提高字符串匹配效率和加快滑动速度上做不同的优化，从而衍生出各种算法。 
 * RK算法-使用哈希算法提高字符串匹配效率，哈希算法的原理是把字符串转换成26进制整数，然后使用比较整数； 
 * 
 * BM算法和KMP算法的核心思想都是，寻找规律，在遇到不匹配的字符时，向前多移动几位。
 * BM算法-反向匹配，坏字符，好后缀；为提高计算效率，对模式串预处理，生成bc数组，suffix数组，prefix数组； 
 * KMP算法-正向匹配，好前缀；为提高计算效率，对模式串预处理，生成next数组；
 *
 * 适用场景：
 * 1)小规模字符串匹配，BF，O(m*n)；
 * 2)大规模/高性能字符串匹配，BM，O(m+n)；
 *
 * 从字符串匹配算法，可以学到很多分析问题和编程的技巧，
 * 1)RK算法，使用滚动窗口和查表优化哈希计算过程；
 * 2)BM算法和KMP算法，使用预处理，减少比较次数；
 * 3)BM算法，把好后缀规则的两个核心问题，等价为求模式串的子串和模式串的公共后缀子串问题；
 * 4)KMP算法，next数组的计算过程，使用f(i-1)推导f(i)，并把求解次长后缀字串等价为求解次长前缀字串问题；
 * 5)厘清思路，推理一般情况，不要总想着带入0,1，。。。常量，在脑子里推导；
 * 6)在代码实现细节上：明确变量的含义，合并对多个结束条件的判断；
 *
 * ----------------------------------------------------
 * Trie 字典树
 * Trie树的本质，就是利用字符串之间的公共前缀，将重复的前缀合并在一起。
 * Trie树的适用场合：
 *   - 字符串的字符集不能太大，前缀重合比较多，否则会非常浪费空间；
 *   - 适合前缀匹配，不适用精确查找；
 * 应用案例：
 *   - 搜索字符串提示
 *   - 编辑器自动补齐
 *   - 路由表匹配
 *   - 股票代码自动提示
 * 问题：
 *   - 对于更加复杂的中文来说，词库中的数据又该如何构建成Trie树呢？
 *   - 如果词库中有很多关键词，在搜索提示的时候，用户输入关键词，作为前缀在Trie树中可以匹配的关键词也有很多，如何选择展示哪些内容呢？
 *   - 像Google这样的搜索引擎，用户单词拼写错误的情况下，Google还是可以使用正确的拼写来做关键词提示，这个又是怎么做到的呢？
 *
 * ----------------------------------------------------
 * AC自动机
 * AC自动机的本质，就是利用字符串之间的公共前缀，在遇到不匹配的字符时，跳转到下一个最长匹配的位置，加快字串前进速度。
 * AC自动机实际上就是在Trie树之上，加了类似KMP的next数组，只不过此处是失败指针罢了。
 * 失败指针的计算是一个递推过程，对Trie树按层遍历。 
 *
 * 适用场景：
 * 1)多模式匹配，例如过滤敏感词，AC；
 */

namespace bm {
  // 字符集大小
  static const int SIZE = 256;

  // b表示模式串，m表示长度，bc数组事先申请好了
  void generateBC(const char b[], int m, int bc[])
  {
    for (int i = 0; i < SIZE; ++i) {
      bc[i] = -1;
    }
    for (int i = 0; i < m; ++i) {     
      bc[b[i]] = i;
    }
  }

  // b表示模式串，m表示长度，suffix，prefix数组事先申请好了
  void generateGS(const char b[], int m, int suffix[], bool prefix[])
  {
    for (int i = 0; i < m; ++i) {
      suffix[i] = -1;
      prefix[i] = false;
    }
    for (int i = 0; i < m - 1; ++i) {  // b[0,i]
      int j = i;
      int k = 0; // 公共后缀长度
      while (j >= 0 && b[j] == b[m - 1 - k]) {
        suffix[++k] = j--; // j表示公共后缀字串在b[0,i]中的起始下标
      }
      if (j == -1) {
        prefix[k] = true;  // 公共后缀字串也是模式串的前缀子串
      }
    }
  }

  // j表示坏字符对应的模式串中的字符下标; m表示模式串长度
  int moveByGS(int j, int m, int suffix[], bool prefix[])
  {
    int k = m - j - 1;  // 好后缀的长度
    if (suffix[k] != -1)
      return j - suffix[k] + 1;
    for (int r = j + 2; r <= m - 1; ++r) {  // 部分匹配
      if (prefix[m - r] == true)
        return r;
    }
    return m;  // 无法匹配
  }

  // a,b表示主串和模式串；n,m表示主串和模式串的长度
  int bm(const char a[], int n, const char b[], int m)
  {
    int bc[SIZE];  // 记录模式串每个字符最后出现的位置
    generateBC(b, m, bc);
    std::unique_ptr<int[]> suffix(new int[m]);
    std::unique_ptr<bool[]> prefix(new bool[m]);
    generateGS(b, m, suffix.get(), prefix.get());

    int i = 0;  // i标识主串与模式串匹配的第一个字符
    while (i <= n - m) {
      int j;
      for (j = m - 1; j >= 0; --j) {  // 模式串重后往前匹配
        if (a[i + j] != b[j])  // 坏字符对应模式串中的下标为j
          break;
      }
      if (j < 0) {  // 匹配成功，返回主串与模式串第一个匹配的字符的下标
        return i;
      }
      int x = j - bc[a[i + j]];  // 坏字符规则：向前移动的步数，有可能出现往回移的情况
      int y = 0;
      if (j < m - 1) {  // 如果有好后缀的话
        y = moveByGS(j, m, suffix.get(), prefix.get());  // 好后缀规则向前移动的步数， >0
      }
      i += std::max(x, y);
    }

    return -1;
  }

  void test()
  {
    const char *a = "abcacabcbcbacabc";
    const char *b = "ba";

    int p = bm::bm(a, strlen(a), b, strlen(b));
    printf("a:%s, b:%s, p:%d\n", a, b, p);
  }
}


namespace kmp {
  void generateNext(const char b[], int m, int next[])
  {
    next[0] = -1;
    int k = -1;  // 最长可匹配后缀子串的结尾字符下标
    for (int i = 1; i < m; ++i)
    {
      while (k != -1 && b[k+1] != b[i]) {
        k = next[k];
      }
      if (b[k + 1] == b[i]) {  // 涵盖退出条件：k==-1和b[k+1]==b[i]
        ++k;
      }
      next[i] = k;
    }
  }

  int kmp(const char a[], int n, const char b[], int m)
  {
    std::unique_ptr<int[]> next(new int[m]);
    generateNext(b, m, next.get());  
    int j = 0;  // j表示模式串的当前匹配下标
    for(int i=0; i<n; ++i) {  // i表示主串的当前匹配下标
      while (j > 0 && a[i] != b[j]) {
        j = next[j - 1] + 1;
      }
      if (a[i] == b[j]) {  // 涵盖退出条件：j==0 和 a[i]==b[j]
        ++j;  // 向前移动j
      }
      if (j == m) {
        return i - m + 1;
      }
    }
    return -1;
  }

  void test()
  {
    const char *a = "abcacabcbcbacabc";
    const char *b = "ba";

    int p = kmp::kmp(a, strlen(a), b, strlen(b));
    printf("a:%s, b:%s, p:%d\n", a, b, p);
  }
}


namespace trie {
  // 字符集大小
  static const int SIZE = 26;

  // 模式串最大长度
  static const int PATTERN_MAX_LENGH = 64;

  struct TrieNode {
    char data;
    bool is_ending;
    TrieNode* children[SIZE];  

    TrieNode(char c)
      : data(c), is_ending(false)
    {
      for (int i = 0; i < SIZE; ++i)
        children[i] = nullptr;
    }
  };

  struct Trie {    
    TrieNode *root = new TrieNode('/');  // 存储无意义的字符
    std::map<TrieNode*, std::string> prefix;  // 模式串列表，方便输出

    // 往Trie树中插入一个字符串
    void insert(const char *s)
    {    
      if (!s) return;
      size_t n = strlen(s);
      TrieNode* p = root;
      for (size_t i = 0; i < n; ++i) {        
        TrieNode* &next = p->children[s[i] - 'a'];
        if (next == nullptr)
          next = new TrieNode(s[i]);
        p = next;
      }
      if (p != root) {
        p->is_ending = true;
        prefix[p] = s;
      }
    }

    // 根据前缀查找匹配项
    std::vector<std::string> find(const char *s)
    {
      std::vector<std::string> items;      
      if (!s)
        return items;
      size_t n = strlen(s);                 
      TrieNode* p = root;      
      for(size_t i=0; i<n; ++i) {
        p = p->children[s[i] - 'a'];
        if (!p) break;
        if (p->is_ending)
          items.push_back(prefix[p]);
      }
      if (p) {  // 按层遍历
        std::queue<TrieNode*> queue;
        queue.push(p);
        while (!queue.empty()) {
          auto *p = queue.front();         
          queue.pop();
          for (int i = 0; i < SIZE; ++i) {
            auto *pc = p->children[i];
            if (pc) {
              if (pc->is_ending)
                items.push_back(prefix[pc]);
              queue.push(pc);
            }
          }
        }
      }
      return items;
    } 
  };


  void test()
  {
    Trie trie;
    trie.insert("helloword");
    trie.insert("hello");
    trie.insert("her");  
    trie.insert("hi");
    trie.insert("so");
    trie.insert("see");
    trie.insert("seeyou");
    
    std::vector<std::string> s = { "hello", "he", "h", "s"};
    for (auto i : s) {
      auto res = trie.find(i.c_str());
      std::cout << "find[" << i << "]";
      for (auto r : res)
        std::cout << "  " << r;
      std::cout << "\n";
    }
  }
}


namespace ac {
  // 字符集大小
  static const int SIZE = 26;

  struct AcNode {
    char data;
    bool is_ending;
    int  length;  // 当is_ending=true时，记录模式串的长度
    AcNode* children[SIZE];
    AcNode* fail;

    AcNode(char c)
      : data(c), is_ending(false), length(0), fail(nullptr)
    {
      for (int i = 0; i < SIZE; ++i)
        children[i] = nullptr;
    }
  };

  struct AcTrie {
    AcNode *root = new AcNode('/');  // 存储无意义的字符

    // 往Trie树中插入一个字符串
    void insert(const char *s)
    {
      if (!s) return;
      size_t n = strlen(s);
      AcNode* p = root;
      for (size_t i = 0; i < n; ++i) {
        AcNode* &next = p->children[s[i] - 'a'];
        if (next == nullptr)
          next = new AcNode(s[i]);
        p = next;
      }
      if (p != root) {
        p->is_ending = true;
        p->length = n;
      }
    }

    // 计算失败指针
    void buildFailPointer()
    {
      std::queue<AcNode*> queue;
      queue.push(root);
      while (!queue.empty()) {
        AcNode *p = queue.front();
        queue.pop();
        for (int i = 0; i < SIZE; ++i) {
          auto *pc = p->children[i];
          if (pc == nullptr) continue;
          if (p == root) {
            pc->fail = root;  // 所有节点的fail指针没有空悬，root节点空指针表示指向自己
          }
          else {
            auto *q = p->fail;
            while (q != nullptr) {
              auto *qc = q->children[i];
              if (qc != nullptr) {
                pc->fail = qc;
                break;
              }
              q = q->fail;
            }
            if (pc->fail == nullptr)
              pc->fail = root;
          }
          queue.push(pc);
        }
      }
    }

    // 匹配模式串
    struct Item { int idx;  int len; };
    std::vector<Item> match(const char *s)
    {
      std::vector<Item> items;
      if (!s) {
        return items;
      }
      int n = strlen(s);
      AcNode* p = root;  // p表示已经匹配的节点
      for (int i = 0; i < n; ++i) {
        char idx = s[i] - 'a';
        if (idx < 0 || idx >= SIZE) {  // 过滤无效字符
          p = root;
          continue;
        }
        while (p->children[idx] == nullptr && p != root)
          p = p->fail;
        p = p->children[idx];
        if (p == nullptr) p = root;  // 如果没有匹配的，从root从头开始匹配
        AcNode *q = p;
        while (q != root) {  // 找出匹配的模式串
          if (q->is_ending)
            items.push_back({ i - q->length + 1, q->length });
          q = q->fail;
        }        
      }
      return items;
    }
  };

  void test()
  {
    AcTrie ac;
    ac.insert("abcd");
    ac.insert("bcd");
    ac.insert("c");
    ac.buildFailPointer();

    const char *s = "hello c, please say abc, bcd and abcd";
    auto items = ac.match(s);
    for (auto i : items) {
      std::cout << i.idx << "," << i.len << ":" << std::string(s, i.idx, i.len) << std::endl;
    }
  }
};