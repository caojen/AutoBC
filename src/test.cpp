#include "ltl/ltl.hpp"

int main() {
  ltl::LTL l = ltl::LTL::parse("a -> t");

  std::cout << l << std::endl;

  auto l2 = ltl::LTL::parse("X a <-> ! b");
  std::cout << l2 << std::endl;

  using namespace ltl;
  BigInteger a1("-102010231");
  BigInteger a2("-9998492");
  std::cout << (a1-a2) << std::endl;
}
