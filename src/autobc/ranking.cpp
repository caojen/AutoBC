#include <algorithm>

#include "ranking.hpp"

using namespace ltl;

namespace autobc {
    RankingItem::RankingItem(const ltl::LTL &ltl, double rank) : ltl(ltl) {
      this->rank = rank;
    }

    bool RankingItem::operator<(const RankingItem& other) const {
      return this->rank < other.rank || (this->rank == other.rank && this->ltl < other.ltl);
    }

    Ranking::Ranking(const std::string &counter, const std::string &jdk16, unsigned bound) {
      this->counter = counter;
      this->jdk16 = jdk16;
      this->mc = std::make_shared<ltl::ModelCounter>(counter, jdk16);
      this->bound = bound;
    }

    std::vector<RankingItem> Ranking::rank(const std::set<ltl::LTL>& domains,
                                           const std::set<ltl::LTL>& goals,
                                           const ltl::LTL& gd,
                                           const std::set<ltl::LTL>& replacements) {
      std::vector<RankingItem> ret;

      for(auto& replacement: replacements) {
        BigInteger numerator;
        BigInteger denominator;
        {
          // 计算分母
          auto ltls = domains;
          for(auto& goal: goals) {
            ltls.insert(goal);
          }
          denominator = this->mc->count(ltls, this->bound);
        }
        {
          // 计算分子
          auto ltls = domains;
          for(auto& goal: goals) {
            if(goal != gd) {
              ltls.insert(goal);
            }
          }
          ltls.insert(replacement);
          numerator = this->mc->count(ltls, this->bound);
        }
        ret.emplace_back( replacement, numerator.device(denominator) );
      }
      std::sort(ret.begin(), ret.end());
      return ret;
    }
}
