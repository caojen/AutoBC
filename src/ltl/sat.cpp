#include "sat.hpp"

namespace ltl {
  SatSolver satSolver;

  bool SatSolver::operator()(const LTL& ltl) {
    auto iter = cache.find(ltl);
    if(iter != cache.end()) {
      return iter->second;
    } else {
      // TODO: call solver
    }
  }
}
