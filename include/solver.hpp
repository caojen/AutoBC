#pragma once

#include <set>

#include "ltl.hpp"

namespace autobc {
  class Solver {
    public:
      Solver();
      // 向求解器添加Goal
      Solver& goal(const LTL& ltl);
      // 向求解器添加Domain
      Solver& domain(const LTL& ltl);
      // 根据给定的Goal和Domain，生成BC，并且排序
           

  };
}