#pragma once

#include <string>
#include <set>
#include <vector>
#include <memory>
#include <algorithm>

#include "ltl.hpp"
#include "fix_solver.hpp"

namespace autobc {

    inline ltl::LTL to_ltl(const ltl::LTL& ltl) { return ltl; }
    inline ltl::LTL to_ltl(const FixResultItem& item) {return item.ltl; }

    template <class T>
    class RankingItem {
    public:
        RankingItem() = delete;
        RankingItem(const T &item, double rank = 0) {
            this->item = item;
            this->rank = rank;
        }

        bool operator<(const RankingItem &other) const {
            return this->rank < other.rank;
        }

        T item;
        double rank;
    };

    template <class T>
    class Ranking {
    public:
        Ranking(const std::string &counter, const std::string &jdk16, unsigned bound = 100) {
            this->counter = counter;
            this->jdk16 = jdk16;
            this->mc = std::make_shared<ltl::ModelCounter>(counter, jdk16);
            this->bound = bound;
        }

        std::vector<RankingItem<T>> rank(const std::set<ltl::LTL> &domains, const std::set<ltl::LTL> &goals, const ltl::LTL &gd, const std::set<T> &replacements) {
            std::vector<RankingItem<T>> ret;

            ltl::BigInteger denominator_a = 0;
            {
                // 计算分母a
                std::set<ltl::LTL> ltls = domains;
                for(auto &goal: goals) {
                    ltls.insert(goal);
                }
                ltls.insert(gd);
                denominator_a = this->mc->count(ltls, this->bound);
            }

            for(auto& replacement: replacements) {
                // 计算replacement的rank
                auto replacement_ltl = to_ltl(replacement);
                ltl::BigInteger numerator = 0;

                if(denominator_a > 0) {
                    // 计算分子
                    std::set<ltl::LTL> ltls = domains;
                    for(auto &goal: goals) {
                        ltls.insert(goal);
                    }
                    ltls.insert(gd);
                    ltls.insert(replacement_ltl);
                    numerator = this->mc->count(ltls, this->bound);
                }

                ltl::BigInteger denominator_b = 0;
                if(numerator > 0) {
                    // 计算分母b
                    std::set<ltl::LTL> ltls = domains;
                    for(auto &goal: goals) {
                        ltls.insert(goal);
                    }
                    ltls.insert(replacement_ltl);
                    denominator_b = this->mc->count(ltls, this->bound);
                }
                std::cout << replacement_ltl << " " << denominator_a << " " << denominator_b << " " << numerator << std::endl;

                if(denominator_a > 0 && denominator_b >= 0 && numerator >= 0) {
                    ret.push_back(RankingItem<T>(replacement, (numerator / denominator_a + numerator / denominator_b) / 2));
                } else {
                    ret.push_back(RankingItem<T>(replacement, -1));
                }
            }

            std::sort(ret.rbegin(), ret.rend());
            
            return ret;
        }
        std::vector<RankingItem<T>> rank(const std::set<ltl::LTL> &domains, const std::set<ltl::LTL> &goals, const ltl::LTL &gd, const std::vector<T> &replacements) {
            std::set<T> forward;
            for(auto& replacement: replacements) {
                forward.insert(replacement);
            }

            return this->rank(domains, goals, gd, forward);
        }

    private:
        std::string counter;
        std::string jdk16;
        std::shared_ptr<ltl::ModelCounter> mc;
        unsigned bound;
    };
}
