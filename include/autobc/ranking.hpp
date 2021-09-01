#pragma once

#include <string>
#include <set>
#include <vector>
#include <algorithm>

#include "ltl.hpp"
#include "model_counter.hpp"

namespace autobc {
  struct RankResultItem {
    ltl::LTL ltl;
    double rank;
    double syn;
    double length;

    bool operator<(const RankResultItem& other) const;
    bool operator==(const RankResultItem& other) const;
  };

  struct OriginResult {
    std::set<ltl::LTL> domains;
    std::set<ltl::LTL> goals;
    ltl::LTL           target;
    std::set<ltl::LTL> replacements;

    static OriginResult parse(const std::string& content);
  };

  class Ranking {
  public:
    Ranking(const std::string& counter, const std::string& jdk16, unsigned bound = 100);
    std::vector<RankResultItem> rank(const std::set<ltl::LTL>& domains, const std::set<ltl::LTL>& goals, const ltl::LTL& target, const std::set<ltl::LTL>& replacements);
    std::vector<RankResultItem> rank(const OriginResult& r);
  private:
    std::string counter;
    std::string jdk16;
    std::shared_ptr<ltl::ModelCounter> mc;
    unsigned bound;
  };

  template<unsigned top>
  std::array<std::set<RankResultItem>, top> get_top(const std::set<RankResultItem>& result) {
    std::array<std::set<RankResultItem>, top> ret;

    std::vector<RankResultItem> vec;
    for(auto& r: result) {
      vec.push_back(r);
    }
    std::sort(vec.begin(), vec.end());

    auto iter = vec.begin();
    for(unsigned i = 0; i < top && iter != vec.end(); i++) {
      while(iter != vec.end()) {
        ret[i].insert(*iter);
        if(std::next(iter) != vec.end()) {
          if(*iter == *std::next(iter)) {
            ++iter;
          } else {
            ++iter;
            break;
          }
        }
      }
    }

    return ret;
  }

  template<unsigned top>
  bool in_top(const std::array<std::set<RankResultItem>, top>& result, const ltl::LTL& formula) {
    for(unsigned i = 0; i < top; i++) {
      for(auto& r: result[i]) {
        if(r.ltl == formula) {
          return true;
        }
      }
    }
    return false;
  }
}
