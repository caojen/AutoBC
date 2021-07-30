#include "ltl.hpp"
#include "sat.hpp"

using namespace ltl;

int main() {
  LTL ltl = LTL::parse("X(X((X((0))) & ((X(!(X(X(!((1) U (G(d)))))))) U (G(X(X(a)))))))");
  std::cout << ltl.serialize() << std::endl;

  satSolver = new SatSolver();

  bool issat = (*satSolver)(ltl);
  std::cout << issat << std::endl;
}
