#include <ctime>
#include <random>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <utility>
#include <string.h>
#include <sys/wait.h>
#include "autobc.hpp"

using namespace ltl;

namespace autobc {
  AutoBC::AutoBC(std::string likelyhood) {
    std::srand(std::time(nullptr));
    this->likelyhood = std::move(likelyhood);
  }

  void AutoBC::add_domain(const Domain &domain) {
    this->domains.insert(domain);
  }

  void AutoBC::add_bc(const BC &bc) {
    this->bcs.insert(bc);
  }

  void AutoBC::add_goal(const Goal &goal) {
    this->goals.insert(goal);
  }

  std::string AutoBC::serialize() const {
    std::ostringstream ostr("");
    ostr << "Domains:" << std::endl;
    for(auto& domain: this->domains) {
      ostr << '\t' << domain << std::endl;
    }

    ostr << "Goals:" << std::endl;
    for(auto& goal: this->goals) {
      ostr << '\t' << goal << std::endl;
    }
    if(!this->sorted) {
      ostr << "BCs:" << std::endl;
      for(auto& bc: this->bcs) {
        ostr << '\t' << bc << std::endl;
      }
    } else {
      ostr << "BCs(sorted):" << std::endl;
      for(unsigned i = 0; i < this->sorted_bcs.size(); i++) {
        ostr << '\t' << this->sorted_bcs[i] << '\t' << this->weight_bcs[i] << std::endl;
      }
    }

    return ostr.str();
  }

  std::ostream& operator<<(std::ostream& o, const AutoBC& ab) {
    o << ab.serialize();
    return o;
  }

  void AutoBC::bc_sort() {
    if(this->bcs.empty()) {
      std::cout << "No BC specified. Ignored...";
      return;
    }
    auto rand_strings = [](unsigned length) -> std::string {
      const char charset[] = "abcdefghijklmnopqrstuvwxyz";
      const unsigned len = sizeof(charset) / sizeof(charset[0]);
      std::string ret;
      for(unsigned i = 0; i < length; i++) {
        ret.append(std::string(1, charset[std::rand() % len]));
      }
      return ret;
    };

    std::string random_prefix = rand_strings(16);
    std::string input_tmp_file = random_prefix + "_input_" + rand_strings(12);
    std::string output_tmp_file = random_prefix + "_output_" + rand_strings(12);

    // 将所有bc以行的方式写入到input_tmp_file
    std::ofstream ofstream;
    ofstream.open(input_tmp_file, std::ios::out | std::ios::trunc);
    auto format_double_and = ltl::format_double_and;
    auto format_double_or = ltl::format_double_or;
    auto format_symbol_F = ltl::format_symbol_F;
    auto format_symbol_G = ltl::format_symbol_G;
    ltl::format_double_and = false;
    ltl::format_double_or = false;
    ltl::format_symbol_F = false;
    ltl::format_symbol_G = false;
    for(auto& bc: this->bcs) {
      ofstream << bc.serialize() << std::endl;
    }

    ofstream.close();

    // 生成命令行
    ltl::format_double_and = true;
    ltl::format_double_or = true;
    ltl::format_symbol_F = true;
    ltl::format_symbol_G = true;
    std::vector<std::string> args;
    args.emplace_back("java");
    args.emplace_back("-jar");
    args.push_back(this->likelyhood);

    for(auto& d: this->domains) {
      args.push_back(std::string("-d=") + d.serialize());
    }
    for(auto& g: this->goals) {
      args.push_back(std::string("-g=") + g.serialize());
    }
    args.push_back(std::string("--bcfile=") + input_tmp_file);
    args.push_back(std::string("--output=") + output_tmp_file);
    args.emplace_back(">/dev/null");

    auto size = args.size();
    char** nargs = new char*[size + 1];
    memset(nargs, 0, sizeof(char*) * (size + 1));

    for(unsigned i = 0; i < size; i++) {
      nargs[i] = new char[args[i].size() + 1];
      auto s = args[i].c_str();
      memcpy(nargs[i], s, args[i].size() * sizeof(char));
      nargs[i][args[i].size()] = 0;
    }
    nargs[size] = (char*)0;

    std::string result;

    auto pid = fork();
    if(pid == 0) {
      int fd = open("/dev/null", O_WRONLY);
      dup2(fd, 1);
      dup2(fd, 2);
      close(fd);
      auto ret = execv(this->jdk8.c_str(), nargs);
      std::cout << "fatal: child returned: " << ret << std::endl;
      std::cout << strerror(errno) << std::endl;
      exit(1);
    } else if(pid > 0) {
      waitpid(pid, nullptr, 0);
      for(unsigned i = 0; i < size; i++) {
        delete[] nargs[i];
      }
      delete[] nargs;
    } else if(pid < 0) {
      std::cout << "fork failed" << std::endl;
      exit(1);
    }

    std::ifstream ifstream;
    ifstream.open(output_tmp_file);
    std::string line;
    while(std::getline(ifstream, line)) {
      if(!line.empty()) {
        result += line + "\n";
      }
    }

    remove(input_tmp_file.c_str());
    remove(output_tmp_file.c_str());
    // 输出结果存放到result中
    // 按行读取：
    auto lines = split(result, "\n");

    if(lines.size() != this->bcs.size()) {
      throw output_line_too_less();
    }
    std::vector<double> weights;
    std::vector<std::string> before_sort;
    for(auto& line: lines) {
      // 每一行的内容：bc, <float>
      auto parts = split(line, ", ");
      if(parts.size() != 2) {
        throw output_line_format_error();
      }
      auto formula = parts[0];
      auto f = parts[1];
      weights.push_back(atof(f.c_str()));
      before_sort.push_back(formula);
    }
    // 选择排序
    while(!weights.empty()) {
      unsigned max_idx = 0;
      for(unsigned i = 0; i < weights.size(); i++) {
        if(weights[i] > weights[max_idx]) {
          max_idx = i;
        }
      }
      this->weight_bcs.push_back(weights[max_idx]);
      weights.erase(weights.begin() + max_idx);
      this->sorted_bcs.emplace_back(ltl::LTL::parse(before_sort[max_idx]));
      before_sort.erase(before_sort.begin() + max_idx);
    }

    this->sorted = true;
    this->target_bc = &this->sorted_bcs[0];
    ltl::format_double_and = format_double_and;
    ltl::format_double_or = format_double_or;
    ltl::format_symbol_F = format_symbol_F;
    ltl::format_symbol_G = format_symbol_G;
  }

  AutoBC AutoBC::parse(const std::string &content) {
    AutoBC ret;
    auto lines = split(content, "\n");

    for(auto &line: lines) {
      if(line.empty()) continue;
      std::size_t pos = 0;
      while(pos < line.size() && line.at(pos) != ':') ++pos;
      ++pos;

      if(pos >= line.size()) {
        throw file_not_valid();
      }

      auto ltls = split(line.substr(pos), ",");
      auto prefix = line.substr(0, pos);
      for(auto &ltl: ltls) {
        if(ltl == " ") {
          continue;
        }
        if(prefix == "Domains:") {
          ret.add_domain(ltl::LTL::parse(ltl));
        } else if(prefix == "Goals:") {
          ret.add_goal(ltl::LTL::parse(ltl));
        } else {
          throw file_not_valid();
        }
      }
    }
    return ret;
  }

  void AutoBC::use_bcs(const std::string& content, bool use_first_line) {
    const auto lines = split(content, "\n");
    unsigned idx = use_first_line ? 0 : 1;
    while(idx < lines.size()) {
      const auto &line = lines.at(idx);
      this->add_bc(ltl::LTL::parse(line));
      idx++;
    }
  }

  std::string AutoBC::into() const {
    auto format_double_and = ltl::format_double_and;
    auto format_double_or = ltl::format_double_or;
    auto format_symbol_F = ltl::format_symbol_F;
    auto format_symbol_G = ltl::format_symbol_G;
    ltl::format_double_and = true;
    ltl::format_double_or = true;
    ltl::format_symbol_F = true;
    ltl::format_symbol_G = true;

    std::ostringstream ostr("");
    ostr << "Domains:";
    for(auto iter = this->domains.begin(); iter != this->domains.end(); ++iter) {
      ostr << " " << iter->serialize();
      if(std::next(iter) != this->domains.end()) {
        ostr << ",";
      }
    }
    ostr << std::endl;
    ostr << "Goals:";
    for(auto iter = this->goals.begin(); iter != this->goals.end(); ++iter) {
      ostr << " " << iter->serialize();
      if(std::next(iter) != this->goals.end()) {
        ostr << ",";
      }
    }
    ltl::format_double_and = format_double_and;
    ltl::format_double_or = format_double_or;
    ltl::format_symbol_F = format_symbol_F;
    ltl::format_symbol_G = format_symbol_G;
    return ostr.str();
  }

  std::string AutoBC::into(const std::string &filename) const {
    auto s = this->into();
    std::ofstream ofstream;
    ofstream.open(filename, std::ios::out | std::ios::trunc);
    ofstream << s;
    ofstream.close();
    return s;
  }

  const Goal &AutoBC::get_fix_goal(unsigned int bound, const std::string& jdk16) {

    ltl::ModelCounter mc(this->modelcounting, jdk16);
    const Goal* ret = nullptr;
    ltl::BigInteger mbi;

    for(auto& goal: this->goals) {
      auto workspace = this->domains;
      workspace.insert(goal);
      workspace.insert(*this->target_bc);
      auto tbi = mc.count(workspace, bound);
      if(!ret || tbi < mbi) {
        ret = &goal;
        mbi = tbi;
      }
    }
    for(auto& domain: this->domains) {
      auto workspace = this->goals;
      workspace.insert(domain);
      workspace.insert(*this->target_bc);
      auto tbi = mc.count(workspace, bound);
      if(!ret || tbi < mbi) {
        ret = &domain;
        mbi = tbi;
      }
    }
    this->target_goal = const_cast<Goal*>(ret);
    return *ret;
  }

  const std::set<ltl::LTL>& AutoBC::fix(unsigned k) {
    this->fixed_goals.clear();
    std::set<LTL> old_goals;
    for(auto& goal: this->goals) {
      if(&goal != this->target_goal) {
        old_goals.insert(goal);
      }
    }
    std::set<LTL> old_domains;
    for(auto& domain: this->domains) {
      if(&domain != this->target_goal) {
        old_domains.insert(domain);
      }
    }
    this->fixSolver = FixSolver(old_domains, *this->target_goal, Lasso(*this->target_bc), old_goals, false);
    this->fixed_goals = this->fixSolver.fix(k);

    return this->fixed_goals;
  }

  const std::set<ltl::LTL>& AutoBC::fix_with_limit(unsigned limit) {
    this->fixed_goals.clear();
    std::set<LTL> old_goals;
    bool goal_is_from_domain = false;
    for(auto& goal: this->goals) {
      if(&goal != this->target_goal) {
        old_goals.insert(goal);
      }
    }
    std::set<LTL> old_domains;
    for(auto& domain: this->domains) {
      if(&domain != this->target_goal) {
        old_domains.insert(domain);
      } else {
        goal_is_from_domain = true;
      }
    }
    this->fixSolver = FixSolver(old_domains, *this->target_goal, Lasso(*this->target_bc), old_goals, goal_is_from_domain);
    this->fixed_goals = this->fixSolver.fix_with_limit(limit);

    return this->fixed_goals;
  }
}

namespace autobc {
  std::vector<std::string> split(const std::string& origin, const std::string& pattern) {
    if(pattern.empty()) {
      return { 1, origin };
    } else {
      std::vector<std::string> ret;
      std::size_t pos = 0;
      while(pos != std::string::npos) {
        auto next = pos;
        next = origin.find(pattern, pos);
        std::string s;
        if(next != std::string::npos && pos != next) {
          s = origin.substr(pos, next - pos);
        } else if(next == std::string::npos) {
          s = origin.substr(pos);
        }
        if(!s.empty()) {
          ret.push_back(s);
        }
        if(next != std::string::npos) {
          pos = next + pattern.size();
        } else {
          pos = std::string::npos;
        }
      }
      return ret;
    }
  }
}
