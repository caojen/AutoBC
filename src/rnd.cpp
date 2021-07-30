#include "rnd.hpp"

namespace rnd {
  Range::Range(unsigned int right) {
    this->left = 0;
    this->right = right;
  }

  Range::Range(unsigned int left, unsigned int right) {
    this->left = left;
    this->right = right;
  }

  Rander::Rander(const RanderTime& rt) {
    this->rt = rt;
    for(unsigned i = 0; i < 26; i++) {
      char ch = 'a' + i;
      this->charset.emplace_back(1, ch);
    }
  }

  void Rander::set_limit(const RanderTime &rt) {
    this->rt = rt;
  }

  std::set<LTLPair> Rander::clear() {
    std::set<LTLPair> ret = std::move(this->ltls);
    this->ltls.clear();
    return ret;
  }

  Rander::~Rander() {}

  RanderCNF::RanderCNF(const RanderTime &rt): Rander(rt) {}

  std::set<LTLPair> RanderCNF::operator()(unsigned k, const Range& r) {
    std::cout << "RanderCNF started. Limited " << this->rt.count() << "s to generate " << k << " LTL pairs ";
    std::cout << "in depth [" << r.left << ", " << r.right << ")." << std::endl;

    std::set<LTLPair> ret;

    return ret;
  }
}
