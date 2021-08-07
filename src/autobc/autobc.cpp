#include <sstream>
#include "autobc.hpp"

namespace autobc {
  void AutoBC::add_domain(const Domain &domain) {
    this->domains.emplace_back(domain);
  }

  void AutoBC::add_bc(const BC &bc) {
    this->bcs.emplace_back(bc);
  }

  void AutoBC::add_goal(const Goal &goal) {
    this->goals.emplace_back(goal);
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

    ostr << "BCs:" << std::endl;
    for(auto& bc: this->bcs) {
      ostr << '\t' << bc << std::endl;
    }

    return ostr.str();
  }

  std::ostream& operator<<(std::ostream& o, const AutoBC& ab) {
    o << ab.serialize();
    return o;
  }

  void AutoBC::bc_get() {
  }

  void AutoBC::bc_sort() {
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
          ret.domains.emplace_back(ltl::LTL::parse(ltl));
        } else if(prefix == "Goals:") {
          ret.goals.emplace_back(ltl::LTL::parse(ltl));
        } else {
          throw file_not_valid();
        }
      }
    }
    return ret;
  }

  std::string AutoBC::into() const {
    auto save = ltl::format_as_symbol;
    ltl::format_as_symbol = true;
    std::ostringstream ostr("");
    ostr << "Domains:";
    for(unsigned i = 0; i < this->domains.size(); i++) {
      ostr << " " << this->domains[i].serialize();
      if(i != this->domains.size() - 1) {
        ostr << ",";
      }
    }
    ostr << std::endl;
    ostr << "Goals:";
    for(unsigned i = 0; i < this->goals.size(); i++) {
      ostr << " " << this->goals[i].serialize();
      if(i != this->goals.size() - 1) {
        ostr << ",";
      }
    }
    ltl::format_as_symbol = save;
    return ostr.str();
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
