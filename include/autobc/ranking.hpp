#pragma once

#include <string>
#include <set>
#include <vector>
#include <memory>

#include "ltl.hpp"

namespace autobc {
  class RankingItem {
  public:
      RankingItem() = delete;
      RankingItem(const ltl::LTL &ltl, double rank);
      RankingItem(const RankingItem&) = default;

      bool operator<(const RankingItem& other) const;

      const ltl::LTL& ltl;
      double rank;
  };

  class Ranking {
  public:
      Ranking(const std::string& counter, const std::string& jdk16, unsigned bound = 100);

      std::vector<RankingItem> rank(const std::set<ltl::LTL>& domains, const std::set<ltl::LTL>& goals, const ltl::LTL& gd, const std::set<ltl::LTL>& replacements);
  private:
      std::string                         counter;
      std::string                         jdk16;
      std::shared_ptr<ltl::ModelCounter>  mc;
      unsigned                            bound;
  };
}
