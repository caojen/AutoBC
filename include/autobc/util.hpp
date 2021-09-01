#pragma once

#include "ltl.hpp"

namespace autobc {
  std::set<ltl::LTL> sf(const ltl::LTL& formula);
  std::set<ltl::LTL> sf(const std::shared_ptr<ltl::LTL::LTLNode>& root);

  ltl::LTL model_ltl(const ltl::LTL& f);
  void to_model(std::shared_ptr<ltl::LTL::LTLNode>& root);
  double synSim(const ltl::LTL& f1, const ltl::LTL& f2, bool use_model);
}
