#pragma once

#include <algorithm>
#include <memory>
#include <queue>
#include <map>

/*
 * ----------------------------------------------------
 * ��ģʽ�ַ���ƥ��

 * ������ƥ���㷨BF��ʼ��������ַ���ƥ��Ч�ʺͼӿ컬���ٶ�������ͬ���Ż����Ӷ������������㷨�� 
 * RK�㷨-ʹ�ù�ϣ�㷨����ַ���ƥ��Ч�ʣ���ϣ�㷨��ԭ���ǰ��ַ���ת����26����������Ȼ��ʹ�ñȽ������� 
 * 
 * BM�㷨��KMP�㷨�ĺ���˼�붼�ǣ�Ѱ�ҹ��ɣ���������ƥ����ַ�ʱ����ǰ���ƶ���λ��
 * BM�㷨-����ƥ�䣬���ַ����ú�׺��Ϊ��߼���Ч�ʣ���ģʽ��Ԥ��������bc���飬suffix���飬prefix���飻 
 * KMP�㷨-����ƥ�䣬��ǰ׺��Ϊ��߼���Ч�ʣ���ģʽ��Ԥ��������next���飻
 *
 * ���ó�����
 * 1)С��ģ�ַ���ƥ�䣬BF��O(m*n)��
 * 2)���ģ/�������ַ���ƥ�䣬BM��O(m+n)��
 *
 * ���ַ���ƥ���㷨������ѧ���ܶ��������ͱ�̵ļ��ɣ�
 * 1)RK�㷨��ʹ�ù������ںͲ���Ż���ϣ������̣�
 * 2)BM�㷨��KMP�㷨��ʹ��Ԥ�������ٱȽϴ�����
 * 3)BM�㷨���Ѻú�׺����������������⣬�ȼ�Ϊ��ģʽ�����Ӵ���ģʽ���Ĺ�����׺�Ӵ����⣻
 * 4)KMP�㷨��next����ļ�����̣�ʹ��f(i-1)�Ƶ�f(i)���������γ���׺�ִ��ȼ�Ϊ���γ�ǰ׺�ִ����⣻
 * 5)����˼·������һ���������Ҫ�����Ŵ���0,1�����������������������Ƶ���
 * 6)�ڴ���ʵ��ϸ���ϣ���ȷ�����ĺ��壬�ϲ��Զ�������������жϣ�
 *
 * ----------------------------------------------------
 * Trie �ֵ���
 * Trie���ı��ʣ����������ַ���֮��Ĺ���ǰ׺�����ظ���ǰ׺�ϲ���һ��
 * Trie�������ó��ϣ�
 *   - �ַ������ַ�������̫��ǰ׺�غϱȽ϶࣬�����ǳ��˷ѿռ䣻
 *   - �ʺ�ǰ׺ƥ�䣬�����þ�ȷ���ң�
 * Ӧ�ð�����
 *   - �����ַ�����ʾ
 *   - �༭���Զ�����
 *   - ·�ɱ�ƥ��
 *   - ��Ʊ�����Զ���ʾ
 * ���⣺
 *   - ���ڸ��Ӹ��ӵ�������˵���ʿ��е������ָ���ι�����Trie���أ�
 *   - ����ʿ����кܶ�ؼ��ʣ���������ʾ��ʱ���û�����ؼ��ʣ���Ϊǰ׺��Trie���п���ƥ��Ĺؼ���Ҳ�кܶ࣬���ѡ��չʾ��Щ�����أ�
 *   - ��Google�������������棬�û�����ƴд���������£�Google���ǿ���ʹ����ȷ��ƴд�����ؼ�����ʾ�����������ô�������أ�
 *
 * ----------------------------------------------------
 * AC�Զ���
 * AC�Զ����ı��ʣ����������ַ���֮��Ĺ���ǰ׺����������ƥ����ַ�ʱ����ת����һ���ƥ���λ�ã��ӿ��ִ�ǰ���ٶȡ�
 * AC�Զ���ʵ���Ͼ�����Trie��֮�ϣ���������KMP��next���飬ֻ�����˴���ʧ��ָ����ˡ�
 * ʧ��ָ��ļ�����һ�����ƹ��̣���Trie����������� 
 *
 * ���ó�����
 * 1)��ģʽƥ�䣬����������дʣ�AC��
 */

namespace bm {
  // �ַ�����С
  static const int SIZE = 256;

  // b��ʾģʽ����m��ʾ���ȣ�bc���������������
  void generateBC(const char b[], int m, int bc[])
  {
    for (int i = 0; i < SIZE; ++i) {
      bc[i] = -1;
    }
    for (int i = 0; i < m; ++i) {     
      bc[b[i]] = i;
    }
  }

  // b��ʾģʽ����m��ʾ���ȣ�suffix��prefix���������������
  void generateGS(const char b[], int m, int suffix[], bool prefix[])
  {
    for (int i = 0; i < m; ++i) {
      suffix[i] = -1;
      prefix[i] = false;
    }
    for (int i = 0; i < m - 1; ++i) {  // b[0,i]
      int j = i;
      int k = 0; // ������׺����
      while (j >= 0 && b[j] == b[m - 1 - k]) {
        suffix[++k] = j--; // j��ʾ������׺�ִ���b[0,i]�е���ʼ�±�
      }
      if (j == -1) {
        prefix[k] = true;  // ������׺�ִ�Ҳ��ģʽ����ǰ׺�Ӵ�
      }
    }
  }

  // j��ʾ���ַ���Ӧ��ģʽ���е��ַ��±�; m��ʾģʽ������
  int moveByGS(int j, int m, int suffix[], bool prefix[])
  {
    int k = m - j - 1;  // �ú�׺�ĳ���
    if (suffix[k] != -1)
      return j - suffix[k] + 1;
    for (int r = j + 2; r <= m - 1; ++r) {  // ����ƥ��
      if (prefix[m - r] == true)
        return r;
    }
    return m;  // �޷�ƥ��
  }

  // a,b��ʾ������ģʽ����n,m��ʾ������ģʽ���ĳ���
  int bm(const char a[], int n, const char b[], int m)
  {
    int bc[SIZE];  // ��¼ģʽ��ÿ���ַ������ֵ�λ��
    generateBC(b, m, bc);
    std::unique_ptr<int[]> suffix(new int[m]);
    std::unique_ptr<bool[]> prefix(new bool[m]);
    generateGS(b, m, suffix.get(), prefix.get());

    int i = 0;  // i��ʶ������ģʽ��ƥ��ĵ�һ���ַ�
    while (i <= n - m) {
      int j;
      for (j = m - 1; j >= 0; --j) {  // ģʽ���غ���ǰƥ��
        if (a[i + j] != b[j])  // ���ַ���Ӧģʽ���е��±�Ϊj
          break;
      }
      if (j < 0) {  // ƥ��ɹ�������������ģʽ����һ��ƥ����ַ����±�
        return i;
      }
      int x = j - bc[a[i + j]];  // ���ַ�������ǰ�ƶ��Ĳ������п��ܳ��������Ƶ����
      int y = 0;
      if (j < m - 1) {  // ����кú�׺�Ļ�
        y = moveByGS(j, m, suffix.get(), prefix.get());  // �ú�׺������ǰ�ƶ��Ĳ����� >0
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
    int k = -1;  // ���ƥ���׺�Ӵ��Ľ�β�ַ��±�
    for (int i = 1; i < m; ++i)
    {
      while (k != -1 && b[k+1] != b[i]) {
        k = next[k];
      }
      if (b[k + 1] == b[i]) {  // �����˳�������k==-1��b[k+1]==b[i]
        ++k;
      }
      next[i] = k;
    }
  }

  int kmp(const char a[], int n, const char b[], int m)
  {
    std::unique_ptr<int[]> next(new int[m]);
    generateNext(b, m, next.get());  
    int j = 0;  // j��ʾģʽ���ĵ�ǰƥ���±�
    for(int i=0; i<n; ++i) {  // i��ʾ�����ĵ�ǰƥ���±�
      while (j > 0 && a[i] != b[j]) {
        j = next[j - 1] + 1;
      }
      if (a[i] == b[j]) {  // �����˳�������j==0 �� a[i]==b[j]
        ++j;  // ��ǰ�ƶ�j
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
  // �ַ�����С
  static const int SIZE = 26;

  // ģʽ����󳤶�
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
    TrieNode *root = new TrieNode('/');  // �洢��������ַ�
    std::map<TrieNode*, std::string> prefix;  // ģʽ���б��������

    // ��Trie���в���һ���ַ���
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

    // ����ǰ׺����ƥ����
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
      if (p) {  // �������
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
  // �ַ�����С
  static const int SIZE = 26;

  struct AcNode {
    char data;
    bool is_ending;
    int  length;  // ��is_ending=trueʱ����¼ģʽ���ĳ���
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
    AcNode *root = new AcNode('/');  // �洢��������ַ�

    // ��Trie���в���һ���ַ���
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

    // ����ʧ��ָ��
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
            pc->fail = root;  // ���нڵ��failָ��û�п�����root�ڵ��ָ���ʾָ���Լ�
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

    // ƥ��ģʽ��
    struct Item { int idx;  int len; };
    std::vector<Item> match(const char *s)
    {
      std::vector<Item> items;
      if (!s) {
        return items;
      }
      int n = strlen(s);
      AcNode* p = root;  // p��ʾ�Ѿ�ƥ��Ľڵ�
      for (int i = 0; i < n; ++i) {
        char idx = s[i] - 'a';
        if (idx < 0 || idx >= SIZE) {  // ������Ч�ַ�
          p = root;
          continue;
        }
        while (p->children[idx] == nullptr && p != root)
          p = p->fail;
        p = p->children[idx];
        if (p == nullptr) p = root;  // ���û��ƥ��ģ���root��ͷ��ʼƥ��
        AcNode *q = p;
        while (q != root) {  // �ҳ�ƥ���ģʽ��
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