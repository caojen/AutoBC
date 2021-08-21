#include <queue>

#include "lasso.hpp"

using namespace ltl;

typedef LTL::LTLNode LTLNode;

namespace autobc {
  Lasso::Lasso(const LTL& ltl) {
    this->ltl = ltl;
    // TODO: Get Lasso terms of this->ltl, insert into this->terms
  }

  std::set<ltl::LTL> Lasso::fetch_terms(unsigned int i) {
    if(i == 0 || i > this->terms.size()) {
      return {};
    }

    if(this->cache.find(i) != this->cache.end()) {
      return this->cache[i];
    } else {
      // 构造C(i, n)

    }
  }
}
