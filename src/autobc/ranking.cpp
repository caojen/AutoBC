#include <algorithm>

#include "ranking.hpp"
#include "autobc.hpp"
#include "util.hpp"

extern bool use_model;

namespace autobc {
  bool RankResultItem::operator<(const RankResultItem& other) const {
    if(this->rank > other.rank) {
      return true;
    } else if(this->rank == other.rank) {
      if(this->syn > other.syn) {
        return true;
      } else if(this->syn == other.syn) {
        if(this->length < other.length) {
          return true;
        } else if(this->length == other.length) {
          return this->ltl < other.ltl;
        }
      }
    }
    return false;
  }

  bool RankResultItem::operator==(const RankResultItem& other) const {
    return this->rank == other.rank
      // && this->syn == other.syn
      && this->length == other.length;
  }

  OriginResult OriginResult::parse(const std::string& content) {
    OriginResult r;
    auto lines = split(content, "\n");
    for(auto& line: lines) {
      if(line == "") { continue; }

      auto parts = split(line, "=");
      if(parts.size() != 2) {
        std::cerr << "Parse Error: " << line << std::endl;
        abort();
      }

      if(parts[0] == "domain") {
        r.domains.emplace(ltl::LTL::parse(parts[1]));
      } else if(parts[0] == "goal") {
        r.goals.emplace(ltl::LTL::parse(parts[1]));
      } else if(parts[0] == "target") {
        r.target = ltl::LTL::parse(parts[1]);
      } else if(parts[0] == "fixed") {
        r.replacements.emplace(ltl::LTL::parse(parts[1]));
      } else {
        std::cerr << "Warning: ignore prefix: " << parts[0] << std::endl;
      }
    }

    return r;
  }

  Ranking::Ranking(const std::string& counter, const std::string& jdk16, unsigned bound) {
    this->counter = counter;
    this->jdk16 = jdk16;
    this->mc = std::make_shared<ltl::ModelCounter>(counter, jdk16);
    this->bound = bound;
  }

  std::vector<RankResultItem> Ranking::rank(const OriginResult& r) {
    return this->rank(r.domains, r.goals, r.target, r.replacements);
  }

  std::vector<RankResultItem> Ranking::rank(const std::set<ltl::LTL>& domains, const std::set<ltl::LTL>& goals, const ltl::LTL& target, const std::set<ltl::LTL>& replacements) {
    std::vector<RankResultItem> r;
    for(auto& replacement: replacements) {
      RankResultItem item;
      item.ltl = replacement;
      // 1.1 计算分母a
      ltl::BigInteger denominator_a = 0;
      {
        std::set<ltl::LTL> ltls = domains;
        for(auto& goal: goals) {
          ltls.insert(goal);
        }
        ltls.insert(target);
        denominator_a = this->mc->count(ltls, this->bound);
      }
      {
        // 1. rank

        
        ltl::BigInteger numerator = 0;
        ltl::BigInteger denominator_b = 0;

        // 1.2 计算分子
        if(denominator_a > 0) {
          std::set<ltl::LTL> ltls = domains;
          for(auto& goal: goals) {
            ltls.insert(goal);
          }
          ltls.insert(target);
          ltls.insert(replacement);
          numerator = this->mc->count(ltls, this->bound);
        }

        // 1.3 计算分母b
        if(denominator_a > 0 && numerator > 0) {
          std::set<ltl::LTL> ltls = domains;
          for(auto& goal: goals) {
            ltls.insert(goal);
          }
          ltls.insert(replacement);
          denominator_b = this->mc->count(ltls, this->bound);
        }

        // 计算rank
        if(denominator_a > 0 && denominator_b > 0 && numerator > 0) {
          item.rank = (numerator / denominator_a + numerator / denominator_b) / 2;
        } else {
          item.rank = -1;
        }
      }
      {
        // 计算synsim
        item.syn = synSim(target, replacement, use_model);
      }
      {
        // 计算length：公式长度差距
        auto r_str = replacement.serialize();
        auto t_str = target.serialize();
        int diff = static_cast<int>(r_str.size()) - static_cast<int>(t_str.size());
        item.length = static_cast<double>(std::abs(diff));
      }
      r.push_back(item);
    }

    std::sort(r.begin(), r.end());
    return r;
  }
}
