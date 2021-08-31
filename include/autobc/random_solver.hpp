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

      static ltl::LTL randOp1(const ltl::LTL& f, std::shared_ptr<ltl::Operator> op);
      static ltl::LTL randOp2(const ltl::LTL& f1, std::shared_ptr<ltl::Operator> op, const ltl::LTL& f2);

      std::set<ltl::LTL>      domains;
      ltl::LTL                goal;
      Lasso                   bc;
      std::set<ltl::LTL>      goals;

      std::set<ltl::LTL>      fix_results;
  };

}
