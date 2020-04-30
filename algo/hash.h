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
   * 一致性哈希
   *
   * 背景: 解决取模哈希在发生节点增/减时，数据大量迁移问题。
   *      假设我们有 k 个机器，数据的哈希值的范围是[0, MAX]。我们将整个范围划分成 m 个小区间（m 远大于 k），每个机器负责 m/k 个小区间。
   *      当有新机器加入的时候，我们就将某几个小区间的数据，从原来的机器中搬移到新的机器中。
   *      这样，既不用全部重新哈希、搬移数据，也保持了各个机器上数据数量的均衡。
   * 方案: 把hash范围进行分片，发生节点增/减时，只影响哈希环上前后两个节点的部分key。
   *
   * 实现：
   * 1) 把hash(node)插入哈希环上，哈希环上第一个大于等于hash(key)的节点，即为key所在的节点;
   * 2) 为防止雪崩和防止数据倾斜，把一个物理节点变成多个虚拟节点;
   *
   * 参考：https://www.jianshu.com/p/4163916a2a8a
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
   * 哈希槽数据分片
   *
   * 假设16384个槽位，每个分片1024个槽位，则一共16个分片，可以支持最多16台机器的负载均衡;
   * 增加/删除节点时，人工或自动迁移分片，进行负载均衡;
   *
   * 应用案例: redis
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