#include "ltl.hpp"

#include <set>

namespace autobc {
  class FixSolver {
    public:
      FixSolver() = delete;
      FixSolver(std::set<ltl::LTL>& domains, ltl::LTL& goal, ltl::LTL& bc);

      const std::set<ltl::LTL>& next();
      // 算法主函数
      static std::set<ltl::LTL> SR(const ltl::LTL& formula, const ltl::LTL& lasso);
      static std::set<ltl::LTL> WR(const ltl::LTL& formula, const ltl::LTL& lasso);
    private:
      std::set<ltl::LTL> domains;
      ltl::LTL goal;
      ltl::LTL bc;

      unsigned k;

      std::set<ltl::LTL> used;
      std::set<ltl::LTL> prev;
  };
}
