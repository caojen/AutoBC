#include <sstream>
#include "autobc.hpp"

namespace autobc {
  void AutoBC::add_domain(const Domain &domain) {
    this->domains.emplace_back(domain);
  }

  void AutoBC::add_bc(const BC &bc) {
    this->bcs.emplace_back(bc);
  }

  void AutoBC::add_goal(const Goal &goal) {
    this->goals.emplace_back(goal);
  }

  std::string AutoBC::serialize() const {
    std::ostringstream ostr("");
    ostr << "Domains:" << std::endl;
    for(auto& domain: this->domains) {
      ostr << '\t' << domain << std::endl;
    }
    ostr << std::endl;

    ostr << "Goals:" << std::endl;
    for(auto& goal: this->goals) {
      ostr << '\t' << goal << std::endl;
    }
    ostr << std::endl;

    ostr << "BCs:" << std::endl;
    for(auto& bc: this->bcs) {
      ostr << '\t' << bc << std::endl;
    }
    ostr << std::endl;

    return ostr.str();
  }

  std::ostream& operator<<(std::ostream& o, const AutoBC& ab) {
    o << ab.serialize();
    return o;
  }

  void AutoBC::bc_get() {
  }

  void AutoBC::bc_sort() {
  }
}
