#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>
#include <spot/tl/formula.hh>
#include <spot/tl/simplify.hh>

#include "sat.hpp"

namespace ltl {
  SatSolver satSolver;

  bool SatSolver::operator()(const LTL& ltl) {
    auto iter = cache.find(ltl);
    if(iter != cache.end()) {
      return iter->second;
    } else {
      // TODO: call solver
      spot::formula formula = spot::parse_formula(ltl.serialize());
    }
  }
}
