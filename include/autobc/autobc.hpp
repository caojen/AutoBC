#pragma once

#include <vector>
#include <string>
#include <iostream>
#include "ltl.hpp"
#include "error.hpp"
#include "autobc_error.hpp"

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
    // 输入已有的文件的内容，然后parse
    // 文件内容错误的时候抛出file_not_valid错误
    // 公式错误的时候抛出not_a_ltl错误
    static AutoBC parse(const std::string& content);

    // 将Domain和Goal生成parse的文件形式
    std::string into() const;
  };
}

namespace autobc {
  std::vector<std::string> split(const std::string& origin, const std::string& pattern);
}
