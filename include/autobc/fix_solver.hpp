#pragma once

#include "ltl.hpp"
#include "lasso.hpp"

#include <set>

namespace autobc {
  struct FixResult {
    ltl::LTL ltl;
    std::shared_ptr<std::vector<FixResult>> wr;
    std::shared_ptr<std::vector<FixResult>> sr;

    FixResult(ltl::LTL ltl) {
      this->ltl = ltl;
      this->wr = std::make_shared<std::vector<FixResult>>();
      this->sr = std::make_shared<std::vector<FixResult>>();
    }

    bool operator<(const FixResult& other) const {
      return this->ltl < other.ltl;
    }
  };

  class FixSolver {
    public:
      FixSolver() = default;
      /**
       * @param domains
       * @param goal 需要修复的goal
       * @param bc 指定的Lasso BC
       * @param old_goals 所有的goals，去除掉需要修复的goal
       */
      FixSolver(const std::set<ltl::LTL>& domains, const ltl::LTL& goal, const Lasso& bc, const std::set<ltl::LTL>& old_goals);

      const std::vector<FixResult>& fix(unsigned level);
      
      // 算法主函数
      static std::set<ltl::LTL> SR(const ltl::LTL& formula, const Lasso& lasso);
      static std::set<ltl::LTL> WR(const ltl::LTL& formula, const Lasso& lasso);
    private:
      std::set<ltl::LTL> domains;
      ltl::LTL goal;
      Lasso bc;
      std::set<ltl::LTL> old_goals;

      unsigned level;

      std::set<ltl::LTL> used;
      std::set<FixResult> prev;
      std::vector<FixResult> fix_result;
  };
}
