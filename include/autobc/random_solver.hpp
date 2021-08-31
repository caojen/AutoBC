#pragma once

#include <set>
#include "ltl.hpp"
#include "lasso.hpp"

namespace autobc {

  class RandomSolver {
    public:
      RandomSolver(const std::set<ltl::LTL>& domains, const ltl::LTL& goal, const Lasso& bc, const std::set<ltl::LTL>& old_goals);
      const std::set<ltl::LTL>& fix_with_limit(unsigned limit);

      static std::set<ltl::LTL> RS(const ltl::LTL& formula);
      bool repair_success(const ltl::LTL& formula);

    private:
      std::set<ltl::LTL>      domains;
      ltl::LTL                goal;
      Lasso                   bc;
      std::set<ltl::LTL>      goals;

      std::set<ltl::LTL>      fix_results;
  };

}
