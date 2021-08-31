#pragma once

#include <set>
#include <vector>
#include "ltl.hpp"

namespace autobc {
  class Lasso {
    public:
      Lasso() = default;
      explicit Lasso(const ltl::LTL& ltl);
      Lasso(const Lasso& other);

      ltl::LTL ltl;
      std::set<ltl::LTL> terms;
      std::vector<ltl::LTL> vec_terms;

      std::map<unsigned, std::vector<ltl::LTL>> cache;
      // 获取C(i, n)对LTL公式，这些公式都是由terms组合而成，之间是以&连接的。
      std::vector<ltl::LTL> fetch_terms(unsigned i);
  };

  class C {
  public:
      C(unsigned i, unsigned n) {
          this->i = i;
          this->n = n;
          if(this->i <= 0 || this->i > n) {
              throw std::runtime_error("Cannot construct C: i <= 0 or i > n will throw an exception");
          }

          this->first = true;
      }

      std::vector<unsigned> next() {
          if(this->first) {
              this->first = false;
              for(unsigned t = 1; t <= i; ++t) {
                  this->cache.push_back(t);
              }
          } else {
              signed ptr = this->cache.size() - 1;
              while(ptr >= 0) {
                  if(this->cache.at(ptr) == n - (this->cache.size() - ptr - 1)) {
                      --ptr;
                  } else {
                      this->cache.at(ptr) += 1;
                      for(unsigned j = ptr + 1; j < this->cache.size(); j++) {
                          this->cache.at(j) = this->cache.at(ptr) + j - ptr;
                      }
                      break;
                  }
              }

              if(ptr < 0) {
                  this->cache = {};
                  this->first = true;
              } else if(this->cache.back() > n) {
                  this->cache = {};
                  this->first = true;
              }
          }

          return this->cache;
      }
  private:
      unsigned                      i;
      unsigned                      n;
      bool                          first;
      std::vector<unsigned>         cache;
  };
}
