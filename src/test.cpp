#include "ltl.hpp"

int main() {
  ltl::LTL l = ltl::LTL::parse("a -> t");

  std::cout << l << std::endl;
}
