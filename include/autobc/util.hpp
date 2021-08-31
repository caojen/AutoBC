#pragma once

#include "ltl.hpp"

namespace autobc {
  unsigned sf(const ltl::LTL& formula);
  unsigned sf(const std::shared_ptr<ltl::LTL::LTLNode>& root);
}
