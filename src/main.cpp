#include <iostream>
#include <fstream>

#include "ltl.hpp"
#include "solver.hpp"

using namespace autobc;

int main() {
  // Domains:

  // G((p & X(p)) -> X(X(! h))) = G(!(p & X(p)) | X(X(!h)))

  // Goals:

  //  G(!m | X(!p))
  //  G(!h | X(p))

  LTL domains[] = {
    LTLGenerator::parse("G(!((p) & (X(p))) | X(X(!(h))))")
  };

  LTL goals[] = {
    LTLGenerator::parse("G((!(m)) | (X(!p)))"),
    LTLGenerator::parse("G((!(h)) | (X(p)))"),
    LTLGenerator::parse("(X(i)) & (((1) U (!(e))) U (i))")
  };

  auto solver = Solver();
  solver.domain(domains[0])
    .goal(goals[0])
    .goal(goals[1])
    .goal(goals[2]);
  
  std::cout << solver << std::endl;
}
