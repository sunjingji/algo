#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>

#include <stdio.h>

namespace hash {

  /*
   * Murmurhash
   */
  inline uint32_t murmur_32_scramble(uint32_t k) {
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    return k;
  }
  uint32_t murmur3_32(const uint8_t* key, size_t len, uint32_t seed)
  {
    uint32_t h = seed;
    uint32_t k;
    /* Read in groups of 4. */
    for (size_t i = len >> 2; i; i--) {
      // Here is a source of differing results across endiannesses.
      // A swap here has no effects on hash properties though.
      k = *((uint32_t*)key);
      key += sizeof(uint32_t);
      h ^= murmur_32_scramble(k);
      h = (h << 13) | (h >> 19);
      h = h * 5 + 0xe6546b64;
    }
    /* Read the rest. */
    k = 0;
    for (size_t i = len & 3; i; i--) {
      k <<= 8;
      k |= key[i - 1];
    }
    // A swap is *not* necessary here because the preceeding loop already
    // places the low bytes in the low places according to whatever endianness
    // we use. Swaps only apply when the memory is copied in a chunk.
    h ^= murmur_32_scramble(k);
    /* Finalize. */
    h ^= len;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
  }

  /*
   * һ���Թ�ϣ
   *
   * ����: ���ȡģ��ϣ�ڷ����ڵ���/��ʱ�����ݴ���Ǩ�����⡣
   *      ���������� k �����������ݵĹ�ϣֵ�ķ�Χ��[0, MAX]�����ǽ�������Χ���ֳ� m ��С���䣨m Զ���� k����ÿ���������� m/k ��С���䡣
   *      �����»��������ʱ�����Ǿͽ�ĳ����С��������ݣ���ԭ���Ļ����а��Ƶ��µĻ����С�
   *      �������Ȳ���ȫ�����¹�ϣ���������ݣ�Ҳ�����˸������������������ľ��⡣
   * ����: ��hash��Χ���з�Ƭ�������ڵ���/��ʱ��ֻӰ���ϣ����ǰ�������ڵ�Ĳ���key��
   *
   * ʵ�֣�
   * 1) ��hash(node)�����ϣ���ϣ���ϣ���ϵ�һ�����ڵ���hash(key)�Ľڵ㣬��Ϊkey���ڵĽڵ�;
   * 2) Ϊ��ֹѩ���ͷ�ֹ������б����һ������ڵ��ɶ������ڵ�;
   *
   * �ο���https://www.jianshu.com/p/4163916a2a8a
   */
  class ConsistentHash {
  public:
    ConsistentHash(int virtualRatio)
      : virtualRatio_(virtualRatio)
    {}

    void addNode(std::string nodeId) {
      NodePtr node = std::make_shared<SkListNode>();
      node->nodeId = nodeId;
      for (int i = 0; i < virtualRatio_; ++i) {
        auto vid = nodeId + std::to_string(i);
        uint32_t hash = murmur3_32((const uint8_t*)vid.data(), vid.length(), k_murmurhash_seed);
        node->virtualNodeHash.push_back(hash);
        hashCycle_[hash] = node;
      }
      nodes_[nodeId] = node;
    }

    void removeNode(std::string nodeId) {
      auto it = nodes_.find(nodeId);
      if (it == nodes_.end())
        return;
      auto node = it->second;
      for (auto hash : node->virtualNodeHash) {
        hashCycle_.erase(hash);
      }
    }

    void findKey(std::string key) {
      uint32_t hash = murmur3_32((const uint8_t*)key.data(), key.length(), k_murmurhash_seed);
      auto it = hashCycle_.lower_bound(hash);
      if (it == hashCycle_.end()) {
        it = hashCycle_.begin();
      }
      printf("findKey[%s] -> %s\n", key.c_str(), it->second->nodeId.c_str());
    }

    void printSlotMap() {
      uint32_t last = 0;
      for (auto it : hashCycle_) {
        printf("slot[%u-%u]: node %s\n", last, it.first, it.second->nodeId.c_str());
        last = it.first + 1;
      }
    }

  private:
    static const int k_murmurhash_seed = 1;

    struct SkListNode {
      std::string nodeId;
      std::vector<uint32_t> virtualNodeHash;
    };
    typedef std::shared_ptr<SkListNode> NodePtr;

    int virtualRatio_;
    std::map<std::string, NodePtr> nodes_;
    std::map<uint32_t, NodePtr> hashCycle_;
  };


  /*
   * ��ϣ�����ݷ�Ƭ
   *
   * ����16384����λ��ÿ����Ƭ1024����λ����һ��16����Ƭ������֧�����16̨�����ĸ��ؾ���;
   * ����/ɾ���ڵ�ʱ���˹����Զ�Ǩ�Ʒ�Ƭ�����и��ؾ���;
   *
   * Ӧ�ð���: redis
   */


  void test()
  {
    ConsistentHash hash(32);

    for (int i = 0; i < 32; ++i) {
      printf("------------------nodes[%d]------------------\n", i + 1);

      hash.addNode(std::string("node") + std::to_string(i));

      hash.findKey("c++");
      hash.findKey("go");
      hash.findKey("nginx");
      hash.findKey("mysql");
      hash.findKey("redis");
      hash.findKey("mongodb");
    }
  }

}