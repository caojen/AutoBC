#pragma once

#include "ltl.hpp"
#include "lasso.hpp"

#include <set>

namespace autobc {
  class FixSolver {
    public:
      FixSolver() = default;
      FixSolver(const std::set<ltl::LTL>& domains, const ltl::LTL& goal, const Lasso& bc);

      const std::set<ltl::LTL>& next();
      // 算法主函数
      static std::set<ltl::LTL> SR(const ltl::LTL& formula, const Lasso& lasso);
      static std::set<ltl::LTL> WR(const ltl::LTL& formula, const Lasso& lasso);
    private:
      std::set<ltl::LTL> domains;
      ltl::LTL goal;
      Lasso bc;

      unsigned k;

      std::set<ltl::LTL> used;
      std::set<ltl::LTL> prev;
  };
}
