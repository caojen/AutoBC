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
    std::set<Domain>     domains;
    std::set<Goal>       goals;
    std::set<BC>         bcs;
    std::vector<BC>      sorted_bcs;
    std::vector<double>  weight_bcs;

    Goal*                target_goal = nullptr;
    BC*                  target_bc = nullptr;

    bool                 sorted = false;

    std::string          likelyhood;                        // likelyhood.jar 的路径
    std::string          javapath = "/usr/bin/java";        // java的路径

    AutoBC(std::string likelyhood = "./Lasso-BC/likelyhood.jar");

    void add_domain(const Domain& domain);
    void add_goal(const Goal& goal);
    void add_bc(const BC& bc);

    std::string serialize() const;
    friend std::ostream& operator<<(std::ostream& o, const AutoBC& ab);

    // 对目前已经有的bc进行排序
    // 将bcs的内容存放到sorted_bcs当中
    void bc_sort();
    // 输入已有的文件的内容，然后parse
    // 文件内容错误的时候抛出file_not_valid错误
    // 公式错误的时候抛出not_a_ltl错误
    static AutoBC parse(const std::string& content);
    // 输入已有bc文件的内容，然后parse
    // 公式错误的时候抛出not_a_ltl错误
    // 第二个参数说明是否忽略第一行的输入内容
    void use_bcs(const std::string& content, bool use_first_line = false);

    // 将Domain和Goal生成parse的文件形式
    std::string into() const;
    // 将into()生成的字符串截断写入文件，返回字符串内容
    std::string into(const std::string& filename) const;

    // 在生成排序的bc之后，根据第一个bc，返回应该需要修复的goal
    const Goal& get_fix_goal(unsigned bound = 100);

    // 生成需要修复的goal之后，进行修复
    void fix(unsigned k);
  };
}

namespace autobc {
  std::vector<std::string> split(const std::string& origin, const std::string& pattern);
}
