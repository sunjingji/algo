#pragma once

#include <functional>
#include <map>
#include <stack>
#include <tuple>
#include <vector>
#include <random>
#include <queue>

/*
 * 基于跳表实现的有序集
 *
 * 应用案例：
 *   假设猎聘网有10万名猎头，每个猎头都可以通过做任务(比如发布职位)来积累积分，然后通过积分来下载简历。
 *   假设你是猎聘网的一名工程师，如何在内存中存储这10万个猎头ID和积分信息，让它能够支持这样几个操作：
 *     1)根据猎头的 ID 快速查找、删除、更新这个猎头的积分信息；
 *     2)查找积分在某个区间的猎头 ID 列表；
 *     3)查找按照积分从小到大排名在第 x 位到第 y 位之间的猎头 ID 列表。
 */
namespace skiplist {

  class SortedSet {
  public:
    // 有序集成员
    typedef std::tuple<std::string, double> Member;

    // 异常
    struct MemberNotExists {};
    struct NotAllowedKey {};

    SortedSet()
    {
      header_ = new SkListNode(kSkListMaxLevel, "-inf", -DBL_MAX);
      tail_ = new SkListNode(kSkListMaxLevel, "+inf", DBL_MAX);

      for (int i = 0; i < kSkListMaxLevel; ++i) {
        header_->levels[i].forward = tail_;
      }
      tail_->backward = header_;

      level_ = 1;

      randGenerator_.seed((unsigned int)time(NULL));
    }

    ~SortedSet()
    {
      while (header_) {
        auto *tmp = header_->levels[0].forward;
        delete header_;
        header_ = tmp;
      }
    }

    // 插入或更新member元素，返回插入的元素数量
    int add(const std::string &member, double score)
    {
      auto *node = findMember(member);

      // 更新
      if (node) {
        if (score != node->score) {
          skListRem(node);
          node->score = score;
          skListInsert(node);
        }
        return 0;
      }

      // 插入
      int newLevel = randomLevel();
      node = new SkListNode(newLevel, member, score);
      dict_[member] = node;
      skListInsert(node);
      return 1;
    }

    // 返回有序集中，成员member的score值
    double score(const std::string &member)
    {
      auto *node = findMember(member);
      if (!node)
        throw MemberNotExists{};
      return node->score;
    }

    // 为有序集的成员member的score值加上增量increment，返回member成员的新score值
    double incrBy(const std::string &member, double increment)
    {
      auto *node = findMember(member);
      if (!node)
        throw MemberNotExists{};
      if (increment == 0.0)
        return node->score;
      skListRem(node);
      node->score += increment;
      skListInsert(node);
      return node->score;
    }

    // 返回有序集的基数
    int card()
    {
      return dict_.size();
    }

    // 返回有序集中，score值在min和max之间(包括score值等于min或max)的成员的数量
    int count(double min, double max)
    {
      return 0;
    }

    // 返回有序集中，指定区间内的成员
    std::vector<Member> range(int start, int stop)
    {
      if (start < 0)
        start = card() + start;
      if (stop < 0)
        stop = card() + stop;

      if (start < 0 || stop < 0)
        return {};

      SkListNode *it = header_;
      int range = -1;  // 截止当前元素it的排名，从0开始，-1为header占位
      for (int i = level_ - 1; i >= 0; --i) {
        while (true) {
          auto *next = it->levels[i].forward;
          if (next == tail_)
            break;
          int next_range = range + next->levels[i].span;
          if (next_range > start)
            break;
          it = next;
          range = next_range;
          if (next_range == start)  // 排名不会重复，查找结束
            goto lable_found;
        }
      }

      if (it == header_)  // 有序集为空时，it指向header_
        header_ = header_->levels[0].forward;

      while (range < start && it != tail_) {
        it = it->levels[0].forward;
        ++range;
      }

    lable_found:
      std::vector<Member> members;
      while (range++ <= stop && it != tail_) {
        members.push_back(Member(it->key, it->score));
        it = it->levels[0].forward;
      }

      return members;
    }

    // 返回有序集中，指定区间内的成员，反向排序
    std::vector<Member> revRange(int start, int stop)
    {
      auto members = range(stop, start);
      std::reverse(members.begin(), members.end());
      return members;
    }

    // 返回有序集中，score值介于min和max之间(包括等于min或max)的成员
    std::vector<Member> rangeByScore(double min, double max)
    {
      SkListNode *it = header_;
      for (int i = level_ - 1; i >= 0; --i) {
        while (true) {
          auto *next = it->levels[i].forward;
          if (next == tail_ || next->score >= min)  // score允许有重复值，避免遗漏
            break;
          it = next;
        }
      }

      if (it == header_)  // min为-DBF_MAX，或有序集为空时, it指向header
        header_ = header_->levels[0].forward;

      while (it->score < min && it != tail_) {
        it = it->levels[0].forward;
      }

      std::vector<Member> members;
      while (it->score <= max && it != tail_) {
        members.push_back(Member(it->key, it->score));
        it = it->levels[0].forward;
      }

      return members;
    }

    // 返回有序集中，score值介于max和min之间(包括等于max或min)的所有的成员，反向排序
    std::vector<Member> revRangeByScore(double max, double min)
    {
      auto members = rangeByScore(min, max);
      std::reverse(members.begin(), members.end());
      return members;
    }

    // 返回成员的排名
    int rank(const std::string &member)
    {
      return {};
    }

    // 返回成员的反向排名
    int revRank(const std::string &member)
    {
      return 0;
    }

    // 移除成员, 返回移除的成员的数量
    int rem(const std::string &member)
    {
      auto *node = findMember(member);
      if (!node)
        return 0;

      skListRem(node);
      delete node;
      dict_.erase(member);
      return 1;
    }

    // 移除有序集中，指定排名(rank)区间内的所有成员，返回被移除成员的数量
    int remRangeByRank(int start, int stop)
    {
      return 0;
    }

    // 移除有序集中，所有score值介于min和max之间(包括等于min或max)的成员，返回被移除成员的数量
    int remRangeByScore(double min, double max)
    {
      return 0;
    }

    // 遍历有序集, 从头开始，回调函数返回值<0时终止
    typedef std::function<int(const std::string &member, double score)> ScanCb;
    void scan(const ScanCb &cb)
    {
      auto *it = header_->levels[0].forward;
      while (it != tail_) {
        if (cb(it->key, it->score) < 0)
          break;
        it = it->levels[0].forward;
      }
    }

    // 输出调试信息
    void debug() {
      for (int i = level_ - 1; i >= 0; --i) {
        printf("level[%d] ", i + 1);
        auto *it = header_->levels[i].forward;
        while (it) {
          printf("=>{%s,%d}", it->key.c_str(), it->levels[i].span);
          it = it->levels[i].forward;
        }
        printf("\n");
      }
    }

  private:
    // 跳表的最高层级
    const int kSkListMaxLevel = 32;

    // 控制建立索引和索引层数的概率，调大该值会创建更多更多层级的索引
    const double kSkListP = 0.25;

    struct SkListNode {
      const std::string key;
      double score;
      SkListNode *backward;
      struct SkListLevel {
        int span = 0;  // 与上一个索引节点之间的链表节点个数，包含本节点
        SkListNode *forward = nullptr;
      };
      std::vector<SkListLevel> levels;

      SkListNode(int level, const std::string &key, double score)
        : key(key),
        score(score),
        backward(nullptr)
      {
        levels.resize(level);
      }
    };

    int randomLevel() {
      int level = 1;
      while ((randGenerator_() & 0xFFFF) < (kSkListP * 0xFFFF))
        level += 1;
      return level < kSkListMaxLevel ? level : kSkListMaxLevel;
    }

    SkListNode* findMember(const std::string &member) {
      auto it = dict_.find(member);
      return it != dict_.end() ? it->second : nullptr;
    }

    // 插入member
    void skListInsert(SkListNode *node)
    {
      int new_level = node->levels.size();
      double score = node->score;

      // 确定每一层的插入位置的前驱节点，及其与header之间的span
      std::vector<SkListNode*> path_nodes(level_);
      std::vector<int> spans(level_);
      SkListNode *it = header_;
      int span = 0;  // 与header之间的span
      for (int i = level_ - 1; i >= 0; --i) {
        while (true) {
          auto *next = it->levels[i].forward;
          if (next->score > score || next == tail_) {
            path_nodes[i] = it;
            spans[i] = span;
            break;
          }
          it = next;
          span += next->levels[i].span;
        }
      }

      // 沿访问路径，从下向上更新各层索引
      for (int i = 0; i < level_; ++i) {
        it = path_nodes[i];
        if (i < new_level) {  // 新增链表/索引节点，更新后续索引的span
          node->levels[i].forward = it->levels[i].forward;
          it->levels[i].forward = node;
          if (i == 0) {  // 链表层
            node->backward = it;
            node->levels[i].forward->backward = node;
            node->levels[i].span = 1;
          }
          else {  // 索引层
            node->levels[i].span = spans[0] + 1 - spans[i];
            node->levels[i].forward->levels[i].span -= (node->levels[i].span - 1);
          }
        }
        else {  // 没有新的索引插入，仅更新后续索引的span
          it->levels[i].forward->levels[i].span += 1;
        }
      }

      // 更新新增的索引层
      for (int i = level_; i < new_level; ++i) {
        header_->levels[i].forward = node;
        node->levels[i].forward = tail_;
        node->levels[i].span = spans[0] + 1;
        tail_->levels[i].span = card() - node->levels[i].span;
      }

      // 更新层级
      if (new_level > level_)
        level_ = new_level;
    }

    // 删除节点
    void skListRem(SkListNode *node)
    {
      SkListNode *it = header_;
      for (int i = level_ - 1; i >= 0; --i) {
        do {
          auto *next = it->levels[i].forward;
          if (next == tail_ || node->score < next->score) {  // tail是每一层的最后一个节点
            next->levels[i].span -= 1;    // 删除的节点在两个索引节点之间，修改span
            break;
          }
          if (node->score == next->score && node->key == next->key) {
            it->levels[i].forward = next->levels[i].forward;
            if (i == 0)  // 链表层
              it->levels[i].forward->backward = it;
            else  // 索引层
              it->levels[i].forward->levels[i].span += node->levels[i].span - 1;
            break;
          }
          it = next;
        } while (it);
      }
    }

    // 遍历成绩在[min,max]之间的节点，回调函数返回值<0时终止
    typedef std::function<int(SkListNode *node)> RangeByScoreVisitCb;
    void skListVisitRangeByScore(const RangeByScoreVisitCb &cb, double min, double max)
    {
      SkListNode *it = header_;
      for (int i = level_ - 1; i >= 0; --i) {
        while (true) {
          auto *next = it->levels[i].forward;
          if (next == tail_ || next->score >= min)  // score允许有重复值，避免遗漏
            break;
          it = next;
        }
      }

      if (it == header_)  // min为-DBF_MAX，或有序集为空时, it指向header
        header_ = header_->levels[0].forward;

      while (it->score < min && it != tail_) {
        it = it->levels[0].forward;
      }

      while (it->score <= max && it != tail_) {
        auto *next = it->levels[0].forward;  // 支持边遍历边删除
        if (cb(it) < 0)
          break;
        it = next;
      }
    }

    // 遍历排名在[start,stop]之间的节点，回调函数返回值<0时终止
    typedef std::function<int(SkListNode *node, int range)> RangeVisitCb;
    std::vector<Member> skListVisitRange(const RangeVisitCb &cb, int start, int stop)
    {
      if (start < 0)
        start = card() + start;
      if (stop < 0)
        stop = card() + stop;

      if (start < 0 || stop < 0)
        return {};

      SkListNode *it = header_;
      int range = -1;  // 截止当前元素it的排名，从0开始，-1为header占位
      for (int i = level_ - 1; i >= 0; --i) {
        while (true) {
          auto *next = it->levels[i].forward;
          if (next == tail_)
            break;
          int next_range = range + next->levels[i].span;
          if (next_range > start)
            break;
          it = next;
          range = next_range;
          if (next_range == start)  // 排名不会重复，查找结束
            goto lable_found;
        }
      }

      if (it == header_)  // 有序集为空时，it指向header_
        header_ = header_->levels[0].forward;

      while (range < start && it != tail_) {
        it = it->levels[0].forward;
        ++range;
      }

    lable_found:
      std::vector<Member> members;
      while (range <= stop && it != tail_) {
        auto *next = it->levels[0].forward;  // 支持边遍历边删除
        if (cb(it, range) < 0)
          break;
        ++range;
        it = next;
      }

      return members;
    }

    std::minstd_rand0 randGenerator_;

    std::map<std::string, SkListNode*> dict_;
    SkListNode *header_;
    SkListNode *tail_;
    int level_;
  };

  void test()
  {
    SortedSet zset;

    std::vector<int> ele = { 18,17,4,5,7,8,1,10,13,16,3,9 };
    char key[16] = { 0 };
    for (size_t i = 0; i < ele.size(); ++i) {
      sprintf(key, "ele[%02d]", ele[i]);
      zset.add(key, ele[i]);
    }
    printf("-----------------------------------------\n");
    zset.debug();

    /*for (size_t i = 0; i < ele.size(); ++i) {
      sprintf(key, "ele[%02d]", ele[i]);
      zset.rem(key);
      printf("-----------------------------------------\n");
      zset.debug();
    }*/

    auto members = zset.rangeByScore(5, 10);
    for (auto m : members) {
      printf("%s %f\n", std::get<0>(m).c_str(), std::get<1>(m));
    }
  }

}