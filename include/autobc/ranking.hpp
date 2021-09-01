#pragma once

#include <string>
#include <set>
#include <vector>

#include "ltl.hpp"
#include "model_counter.hpp"

namespace autobc {
  struct RankResultItem {
    ltl::LTL ltl;
    double rank;
    double syn;
    double length;

    bool operator<(const RankResultItem& other) const;
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
}
