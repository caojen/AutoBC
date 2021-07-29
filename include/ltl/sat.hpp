#pragma once

#include <map>

#include "ltl.hpp"

namespace ltl {
  class SatSolver {
    public:
      SatSolver() = default;

      bool operator()(const LTL& ltl);

    private:
      std::map<LTL, bool> cache;
  };

  extern SatSolver satSolver;
}
