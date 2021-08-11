#pragma once

#include <set>
#include "ltl.hpp"

namespace autobc {
  class Lasso {
    public:
      Lasso() = default;
      explicit Lasso(const ltl::LTL& ltl);

      std::set<std::shared_ptr<ltl::Literal>> terms;
      ltl::LTL to;  // 'and' all terms
  };
}
