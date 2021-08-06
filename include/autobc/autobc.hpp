#pragma once

#include <vector>
#include <string>
#include <iostream>
#include "ltl.hpp"
#include "error.hpp"

namespace autobc {
  typedef ltl::LTL Domain;
  typedef ltl::LTL Goal;
  typedef ltl::LTL BC;

  class AutoBC {
  public:
    std::vector<Domain>     domains;
    std::vector<Goal>       goals;
    std::vector<BC>         bcs;

    void add_domain(const Domain& domain);
    void add_goal(const Goal& goal);
    void add_bc(const BC& bc);

    std::string serialize() const;
    friend std::ostream& operator<<(std::ostream& o, const AutoBC& ab);

    // 根据已有的domain和goal，获得bc
    void bc_get();
    // 对目前已经有的bc进行排序
    void bc_sort();
  };
}
