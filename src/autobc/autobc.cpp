#include <sstream>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include "autobc.hpp"

namespace autobc {
  AutoBC::AutoBC(std::string likelyhood) {
    this->likelyhood = likelyhood;
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
    std::string input_tmp_file = random_prefix + rand_strings(12);
    std::string output_tmp_file = random_prefix + rand_strings(12);

    // 将所有bc以行的方式写入到input_tmp_file
    std::ofstream ofstream;
    ofstream.open(input_tmp_file, std::ios::out | std::ios::trunc);
    auto format = ltl::format_as_symbol;
    ltl::format_as_symbol = false;
    for(auto& bc: this->bcs) {
      ofstream << bc.serialize() << std::endl;
    }

    ofstream.close();

    // 生成命令行
    ltl::format_as_symbol = true;
    std::ostringstream ostr("java -jar ");
    ostr << this->likelyhood << " ";
    for(auto& d: this->domains) {
      ostr << "\"-d=" << d.serialize() << "\" ";
    }
    for(auto& g: this->goals) {
      ostr << "\"-g=" << g.serialize() << "\" ";
    }
    ostr << "\"--bcfile=" << input_tmp_file << "\" ";
    ostr << "\"--output=" << output_tmp_file << "\"";
    auto cmd = ostr.str();
    ltl::format_as_symbol = format;

    // 运行cmd，直接从标准输出获取内容，不再读取output_tmp_file
    FILE* pipe = popen(cmd.c_str(), "r");
    if(!pipe) {
      throw std::runtime_error(strerror(errno));
    }
    std::string result;
    char buffer[10240] = { 0 };
    while(fgets(buffer, sizeof(buffer), pipe) != NULL) {
      result.append(buffer);
    }
    pclose(pipe);
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
    for(unsigned i = 0; i < weights.size(); i++) {
      auto max_idx = 0;
      for(unsigned j = 0; j < weights.size(); j++) {
        if(weights[j] > weights[max_idx]) {
          max_idx = j;
        }
      }
      this->weight_bcs.push_back(weights[max_idx]);
      weights.erase(weights.begin() + max_idx);
      this->sorted_bcs.emplace_back(ltl::LTL::parse(before_sort[max_idx]));
      before_sort.erase(before_sort.begin() + max_idx);
    }

    this->sorted = true;
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
    auto idx = use_first_line ? 0 : 1;
    while(idx < lines.size()) {
      const auto &line = lines.at(idx);
      this->add_bc(ltl::LTL::parse(line));
      idx++;
    }
  }

  std::string AutoBC::into() const {
    auto save = ltl::format_as_symbol;
    ltl::format_as_symbol = true;
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
    ltl::format_as_symbol = save;
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
        if(next != std::string::npos && pos != next) {
          ret.emplace_back(origin.substr(pos, next - pos));
        } else if(next == std::string::npos) {
          ret.emplace_back(origin.substr(pos));
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
