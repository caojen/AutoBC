#pragma once

#include <set>
#include <utility>
#include <chrono>

#include "ltl.hpp"

// 随机生成 不同的 k 个 深度最高为 level 的 sat-unsat LTL公式组合
// 重复调用同一个对象不会生成已生成过的公式
// 除非调用clear

namespace rnd {
  typedef std::pair<ltl::LTL, ltl::LTL> LTLPair;

  class Rander {
    public:
      Rander(std::chrono::duration<long long> limit = std::chrono::seconds(60));

      // 禁用拷贝构造函数（性能原因）
      Rander(const Rander& other) = delete;
      // 可以使用移动构造
      Rander(Rander&& rander);

      // 重设单次生成限制时间
      virtual void set_limit(std::chrono::duration<long long> limit = std::chrono::seconds(60));

      // 生成器随机生成k个深度为level的LTL公式
      virtual std::set<LTLPair> operator()(unsigned k, unsigned level) = 0;

      // 清空生成器的所有LTL公式，并将这些公式返回
      virtual std::set<LTLPair> clear();

      virtual ~Rander();

    protected:

      std::chrono::duration<long long> limit;

      std::set<LTLPair> ltls;
  };

  class RanderCNF: public Rander {
    public:
      RanderCNF(std::chrono::duration<long long> limit = std::chrono::seconds(60));

      // 生成器随机生成k个深度为level的LTL公式
      std::set<LTLPair> operator()(unsigned k, unsigned level);
  };

  class RanderNormal: public Rander {
    public:
      RanderNormal(std::chrono::duration<long long> limit = std::chrono::seconds(60));

      std::set<LTLPair> operator()(unsigned k, unsigned level);
  };
}
