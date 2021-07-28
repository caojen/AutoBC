#include <iostream>

#include "ltl.hpp"

using namespace autobc;

int main() {
  // (f1 & f2 | f3) & f4 & G (f3)
  {
    auto f1 = Literal("f1");
    auto f2 = Literal("f2");
    auto f3 = Literal("f3");
    auto f4 = Literal("f4");
    LTL f = LTL::Gen(f1).aand(f2).oor(f3).aand(f4).aand(LTL::Gen(f3).global());

    std::cout << LTL::Gen(f3).global() << std::endl;

    std::cout << f << std::endl;
  }
}
