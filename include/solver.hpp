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
      Solver& genBC();
      // 在调用genBC的基础上，找到第一个BC，然后找到需要处理的goal
      Solver& findGoal();
      // 根据domain，bc，goal，求解算法
      Solver& solve();

      std::set<LTL>                     goals;
      std::set<LTL>                     domains;

      std::set<LTL>                     bcs;

      // 第一个BC
      LTL*                              best_bc        = nullptr;
      // 需要处理的goal
      LTL*                              best_goal      = nullptr;

      friend std::ostream& operator<<(std::ostream& o, const Solver& solver);

      std::string serialize() const;
  };
}
