#pragma once

#include "ltl.hpp"

namespace autobc {
  std::set<ltl::LTL> sf(const ltl::LTL& formula);
  std::set<ltl::LTL> sf(const std::shared_ptr<ltl::LTL::LTLNode>& root);

  double synSim(const ltl::LTL& f1, const ltl::LTL& f2);
}
