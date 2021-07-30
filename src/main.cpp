#include "ltl.hpp"
#include "sat.hpp"

using namespace ltl;

int main() {
  LTL ltl = LTL::parse("XXXXFa&s");
  std::cout << ltl.serialize() << std::endl;

  satSolver(ltl);
}
