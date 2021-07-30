#include "ltl.hpp"
#include "sat.hpp"
#include "rnd.hpp"

using namespace ltl;
using namespace rnd;

int main() {
  std::srand(std::time(nullptr));
  satSolver = new SatSolver();

  LTL ltl = LTL::parse("XXXXFa&s");
  std::cout << "depth: " << ltl.depth() << std::endl;

  auto s15 = std::chrono::seconds(15);
  Rander& rander = *(new RanderCNF(s15));

  auto res = rander(10, Range(3, 100));

  for(auto& pair: res) {
    std::cout << "sat: " << pair.first << std::endl;
    std::cout << "uns: " << pair.second << std::endl;
  }
}
