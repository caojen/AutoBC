#include "ltl.hpp"

using namespace ltl;

int main() {
  LTL ltl = LTL::parse("(f) & (X((G(a)) U (X(X(j)))))");
}
