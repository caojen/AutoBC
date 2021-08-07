#include "ltl.hpp"
#include "autobc.hpp"

using namespace ltl;
using namespace autobc;

int main() {
  auto abc = AutoBC::parse("Domains: [] (r0 && ! g0 -> X r0), [](<> (! (r1 && g1))), [] (! r0 && g0 -> X (! r0)), [] (! r1 && g1 -> X (! r1)), [](<> (! (r0 && g0))), [] (r1 && ! g1 -> X r1)\n"
                           "Goals: [] (! g0 && true || (true && (! g1)) && (r0 && X r1 -> X (X (g0 && g1)))), [] (r0 -> <> g0), [] (r1 -> <> g1)");
  std::cout << abc.into() << std::endl;
}
