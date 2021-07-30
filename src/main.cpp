#include "ltl.hpp"
#include "sat.hpp"
#include "rnd.hpp"

using namespace ltl;
using namespace rnd;

int main() {
  auto s15 = std::chrono::seconds(15);
  Rander& rander = *(new RanderCNF(s15));

  auto res = rander(10, Range(3, 20));

  for(auto& pair: res) {
    std::cout << "sat: " << pair.first << std::endl;
    std::cout << "uns: " << pair.second << std::endl;
  }
}
