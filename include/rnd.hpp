#pragma once

#include <set>
#include <utility>
#include <chrono>
#include <vector>

#include "ltl.hpp"

// 随机生成 不同的 k 个 深度范围为[range.left, range.right) 的 sat-unsat LTL公式组合
// 重复调用同一个对象不会生成已生成过的公式
// 除非调用clear

namespace rnd {
  typedef std::pair<ltl::LTL, ltl::LTL> LTLPair;
  typedef std::chrono::duration<long long> RanderTime;

  class Range {
  public:
      Range(unsigned right);
      Range(unsigned left, unsigned right);

      unsigned left;
      unsigned right;

      inline bool in(unsigned i) const {
        return i >= left && i < right;
      }
  };

  class Rander {
    public:
      Rander(const RanderTime& rt);

      // 重设单次生成限制时间
      virtual void set_limit(const RanderTime& rt);

      // 生成器随机生成k个深度在r范围内的LTL公式
      virtual std::set<LTLPair> operator()(unsigned k, const Range& r) = 0;

      // 清空生成器的所有LTL公式，并将这些公式返回
      virtual std::set<LTLPair> clear();

      virtual ~Rander();

    protected:

      std::vector<std::string> charset;

      RanderTime rt;

      std::set<LTLPair> ltls;
  };

  class RanderCNF: public Rander {
  public:
    RanderCNF(const RanderTime& rt);
    std::set<LTLPair> operator()(unsigned k, const Range& r);
  };
}
