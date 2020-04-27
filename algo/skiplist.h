#pragma once

#include <functional>
#include <map>
#include <stack>
#include <tuple>
#include <vector>
#include <random>
#include <queue>

/*
 * ��������ʵ�ֵ�����
 *
 * Ӧ�ð�����
 *   ������Ƹ����10������ͷ��ÿ����ͷ������ͨ��������(���緢��ְλ)�����ۻ��֣�Ȼ��ͨ�����������ؼ�����
 *   ����������Ƹ����һ������ʦ��������ڴ��д洢��10�����ͷID�ͻ�����Ϣ�������ܹ�֧����������������
 *     1)������ͷ�� ID ���ٲ��ҡ�ɾ�������������ͷ�Ļ�����Ϣ��
 *     2)���һ�����ĳ���������ͷ ID �б�
 *     3)���Ұ��ջ��ִ�С���������ڵ� x λ���� y λ֮�����ͷ ID �б�
 */
namespace skiplist {

  class SortedSet {
  public:
    // ���򼯳�Ա
    typedef std::tuple<std::string, double> Member;

    // �쳣
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

    // ��������memberԪ�أ����ز����Ԫ������
    int add(const std::string &member, double score)
    {
      auto *node = findMember(member);

      // ����
      if (node) {
        if (score != node->score) {
          skListRem(node);
          node->score = score;
          skListInsert(node);
        }
        return 0;
      }

      // ����
      int newLevel = randomLevel();
      node = new SkListNode(newLevel, member, score);
      dict_[member] = node;
      skListInsert(node);
      return 1;
    }

    // ���������У���Աmember��scoreֵ
    double score(const std::string &member)
    {
      auto *node = findMember(member);
      if (!node)
        throw MemberNotExists{};
      return node->score;
    }

    // Ϊ���򼯵ĳ�Աmember��scoreֵ��������increment������member��Ա����scoreֵ
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

    // �������򼯵Ļ���
    int card()
    {
      return dict_.size();
    }

    // ���������У�scoreֵ��min��max֮��(����scoreֵ����min��max)�ĳ�Ա������
    int count(double min, double max)
    {
      return 0;
    }

    // ���������У�ָ�������ڵĳ�Ա
    std::vector<Member> range(int start, int stop)
    {
      if (start < 0)
        start = card() + start;
      if (stop < 0)
        stop = card() + stop;

      if (start < 0 || stop < 0)
        return {};

      SkListNode *it = header_;
      int range = -1;  // ��ֹ��ǰԪ��it����������0��ʼ��-1Ϊheaderռλ
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
          if (next_range == start)  // ���������ظ������ҽ���
            goto lable_found;
        }
      }

      if (it == header_)  // ����Ϊ��ʱ��itָ��header_
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

    // ���������У�ָ�������ڵĳ�Ա����������
    std::vector<Member> revRange(int start, int stop)
    {
      auto members = range(stop, start);
      std::reverse(members.begin(), members.end());
      return members;
    }

    // ���������У�scoreֵ����min��max֮��(��������min��max)�ĳ�Ա
    std::vector<Member> rangeByScore(double min, double max)
    {
      SkListNode *it = header_;
      for (int i = level_ - 1; i >= 0; --i) {
        while (true) {
          auto *next = it->levels[i].forward;
          if (next == tail_ || next->score >= min)  // score�������ظ�ֵ��������©
            break;
          it = next;
        }
      }

      if (it == header_)  // minΪ-DBF_MAX��������Ϊ��ʱ, itָ��header
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

    // ���������У�scoreֵ����max��min֮��(��������max��min)�����еĳ�Ա����������
    std::vector<Member> revRangeByScore(double max, double min)
    {
      auto members = rangeByScore(min, max);
      std::reverse(members.begin(), members.end());
      return members;
    }

    // ���س�Ա������
    int rank(const std::string &member)
    {
      return {};
    }

    // ���س�Ա�ķ�������
    int revRank(const std::string &member)
    {
      return 0;
    }

    // �Ƴ���Ա, �����Ƴ��ĳ�Ա������
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

    // �Ƴ������У�ָ������(rank)�����ڵ����г�Ա�����ر��Ƴ���Ա������
    int remRangeByRank(int start, int stop)
    {
      return 0;
    }

    // �Ƴ������У�����scoreֵ����min��max֮��(��������min��max)�ĳ�Ա�����ر��Ƴ���Ա������
    int remRangeByScore(double min, double max)
    {
      return 0;
    }

    // ��������, ��ͷ��ʼ���ص���������ֵ<0ʱ��ֹ
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

    // ���������Ϣ
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
    // �������߲㼶
    const int kSkListMaxLevel = 32;

    // ���ƽ������������������ĸ��ʣ������ֵ�ᴴ���������㼶������
    const double kSkListP = 0.25;

    struct SkListNode {
      const std::string key;
      double score;
      SkListNode *backward;
      struct SkListLevel {
        int span = 0;  // ����һ�������ڵ�֮�������ڵ�������������ڵ�
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

    // ����member
    void skListInsert(SkListNode *node)
    {
      int new_level = node->levels.size();
      double score = node->score;

      // ȷ��ÿһ��Ĳ���λ�õ�ǰ���ڵ㣬������header֮���span
      std::vector<SkListNode*> path_nodes(level_);
      std::vector<int> spans(level_);
      SkListNode *it = header_;
      int span = 0;  // ��header֮���span
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

      // �ط���·�����������ϸ��¸�������
      for (int i = 0; i < level_; ++i) {
        it = path_nodes[i];
        if (i < new_level) {  // ��������/�����ڵ㣬���º���������span
          node->levels[i].forward = it->levels[i].forward;
          it->levels[i].forward = node;
          if (i == 0) {  // �����
            node->backward = it;
            node->levels[i].forward->backward = node;
            node->levels[i].span = 1;
          }
          else {  // ������
            node->levels[i].span = spans[0] + 1 - spans[i];
            node->levels[i].forward->levels[i].span -= (node->levels[i].span - 1);
          }
        }
        else {  // û���µ��������룬�����º���������span
          it->levels[i].forward->levels[i].span += 1;
        }
      }

      // ����������������
      for (int i = level_; i < new_level; ++i) {
        header_->levels[i].forward = node;
        node->levels[i].forward = tail_;
        node->levels[i].span = spans[0] + 1;
        tail_->levels[i].span = card() - node->levels[i].span;
      }

      // ���²㼶
      if (new_level > level_)
        level_ = new_level;
    }

    // ɾ���ڵ�
    void skListRem(SkListNode *node)
    {
      SkListNode *it = header_;
      for (int i = level_ - 1; i >= 0; --i) {
        do {
          auto *next = it->levels[i].forward;
          if (next == tail_ || node->score < next->score) {  // tail��ÿһ������һ���ڵ�
            next->levels[i].span -= 1;    // ɾ���Ľڵ������������ڵ�֮�䣬�޸�span
            break;
          }
          if (node->score == next->score && node->key == next->key) {
            it->levels[i].forward = next->levels[i].forward;
            if (i == 0)  // �����
              it->levels[i].forward->backward = it;
            else  // ������
              it->levels[i].forward->levels[i].span += node->levels[i].span - 1;
            break;
          }
          it = next;
        } while (it);
      }
    }

    // �����ɼ���[min,max]֮��Ľڵ㣬�ص���������ֵ<0ʱ��ֹ
    typedef std::function<int(SkListNode *node)> RangeByScoreVisitCb;
    void skListVisitRangeByScore(const RangeByScoreVisitCb &cb, double min, double max)
    {
      SkListNode *it = header_;
      for (int i = level_ - 1; i >= 0; --i) {
        while (true) {
          auto *next = it->levels[i].forward;
          if (next == tail_ || next->score >= min)  // score�������ظ�ֵ��������©
            break;
          it = next;
        }
      }

      if (it == header_)  // minΪ-DBF_MAX��������Ϊ��ʱ, itָ��header
        header_ = header_->levels[0].forward;

      while (it->score < min && it != tail_) {
        it = it->levels[0].forward;
      }

      while (it->score <= max && it != tail_) {
        auto *next = it->levels[0].forward;  // ֧�ֱ߱�����ɾ��
        if (cb(it) < 0)
          break;
        it = next;
      }
    }

    // ����������[start,stop]֮��Ľڵ㣬�ص���������ֵ<0ʱ��ֹ
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
      int range = -1;  // ��ֹ��ǰԪ��it����������0��ʼ��-1Ϊheaderռλ
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
          if (next_range == start)  // ���������ظ������ҽ���
            goto lable_found;
        }
      }

      if (it == header_)  // ����Ϊ��ʱ��itָ��header_
        header_ = header_->levels[0].forward;

      while (range < start && it != tail_) {
        it = it->levels[0].forward;
        ++range;
      }

    lable_found:
      std::vector<Member> members;
      while (range <= stop && it != tail_) {
        auto *next = it->levels[0].forward;  // ֧�ֱ߱�����ɾ��
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