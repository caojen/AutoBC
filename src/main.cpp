#include "ltl.hpp"

using namespace ltl;

int main() {
  LTL ltl = LTL::parse("XXFXGXa&s");
  std::cout << ltl.serialize() << std::endl;
}
