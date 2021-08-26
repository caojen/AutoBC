#pragma once

#include "ltl.hpp"
#include "lasso.hpp"

#include <string>
#include <set>

namespace autobc {
  struct FixResultItem {
    ltl::LTL      ltl;
    std::string   label;

    FixResultItem() {}

    FixResultItem(const ltl::LTL& ltl, const std::string& label = "") {
      this->ltl = ltl;
      this->label = label;
    }

    FixResultItem(const FixResultItem& other) {
      this->label = other.label;
      this->ltl = other.ltl;
    }

    bool operator<(const FixResultItem& other) const {
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

      const std::vector<FixResultItem>& fix(unsigned level);
      const std::vector<FixResultItem>& fix_with_limit(unsigned limit);
      
      // 算法主函数
      static std::set<ltl::LTL> SR(const ltl::LTL& formula, Lasso& lasso, unsigned level, unsigned sublevel);
      static std::set<ltl::LTL> WR(const ltl::LTL& formula, Lasso& lasso, unsigned level, unsigned sublevel);
    private:
      std::set<ltl::LTL> domains;
      ltl::LTL goal;
      Lasso bc;
      std::set<ltl::LTL> old_goals;

      unsigned level;

      std::set<ltl::LTL> used;
      std::set<FixResultItem> prev;
      std::vector<FixResultItem> fix_result;
  };
}
