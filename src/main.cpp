#include <iostream>

#include "ltl.hpp"

using namespace autobc;

int main() {
  // f1 & f2
  {
    auto f1 = Literal("f1");
    auto f2 = Literal("f2");
    LTL f = LTL::Gen(f1).aand(f2);
    std::cout << f << std::endl;
  }
}
