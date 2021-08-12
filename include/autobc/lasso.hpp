#pragma once

#include <set>
#include "ltl.hpp"

namespace autobc {
  class Lasso {
    public:
      Lasso() = default;
      explicit Lasso(const ltl::LTL& ltl);

      std::set<std::pair<bool, std::shared_ptr<ltl::Literal>>> terms;
      std::set<std::shared_ptr<ltl::Literal>> literals;
      ltl::LTL to;  // 'and' all terms
      bool always_false; // to is false(unsat)?
  };
}
