#include "solver.hpp"

namespace autobc {
  Solver::Solver() = default;

  Solver& Solver::goal(const LTL& ltl) {
    const unsigned size = this->goals.size();
    this->goals.insert(LTL(ltl));
    if(size == this->goals.size()) {
      std::cout << "[Warning] Insert an existed Goal: " << ltl.serialize() << std::endl;
    }

    return *this;
  }

  Solver& Solver::domain(const LTL& ltl) {
    const unsigned size = this->domains.size();
    this->domains.insert(LTL(ltl));
    if(size == this->domains.size()) {
      std::cout << "[Warning] Insert an existed Domain: " << ltl.serialize() << std::endl;
    }

    return *this;
  }

  Solver& Solver::genBC() {
    return *this;
  }

  Solver& Solver::findGoal() {
    return *this;
  }

  Solver& Solver::solve() {
    return *this;
  }

  std::ostream& operator<<(std::ostream& o, const Solver& solver) {
    o << solver.serialize();
    return o;
  }

  std::string Solver::serialize() const {
    std::ostringstream ostr("");

    ostr << "[Solver] " << this << std::endl;
    ostr << "    [BC] size = " << this->bcs.size() << std::endl;
    for(auto& bc: this->bcs) {
      ostr << "   [BC] " << bc.serialize() << std::endl;
    }
    ostr << "    [Go] size = " << this->goals.size() << std::endl;
    for(auto& go: this->goals) {
      ostr << "   [Go] " << go.serialize() << std::endl;
    }
    ostr << "    [Do] size = " << this->domains.size() << std::endl;
    for(auto& dom: this->domains) {
      ostr << "   [Do] " << dom.serialize() << std::endl;
    }
    if(this->best_bc) {
      ostr << "   [BestBC] " << this->best_bc->serialize() << std::endl;
    }
    if(this->best_goal) {
      ostr << "   [BestGo] " << this->best_goal << std::endl;
    }

    return ostr.str();
  }
}
