#include "ltl.hpp"
#include "autobc.hpp"
#include "fix_solver.hpp"
#include "lasso.hpp"

using namespace ltl;
using namespace autobc;

int main() {
//   auto abc = AutoBC::parse("Domains: []((p && X(p)) -> X(X(! h)))\n"
//                            "Goals: [](h -> X(p)), [](m -> X(! p))");
//   abc.likelyhood = "../Lasso-BC/likelyhood.jar";
//   abc.javapath = "/usr/bin/java";
//   abc.use_bcs("minepump\n"
//               "(F( h & m))\n"
//               "(F( p & X(G(h  & m & !p | m & p))))\n"
//               "(F(h & p & X(G(h  & !p | !h & m & p | h ))))\n"
//               "(F( !p & X(G(h  & m & !p | !h & m & p | h & p))))\n"
//               "(F(h & m))\n"
//               "(F(h & p & X(G(h  & !p | m & p))))\n"
//               "(F( !p & X(G( h & m | m & p))))\n"
//               "(F( p & X(G(h  & m & !p | !h & m & p | h & p))))\n"
//               "(F(h & m ))\n"
//               "(F( p & X(G(h  & m | m & p))))\n"
//               "(F(h & p & X(G(h  & !m | !h & m & p | h ))))\n"
//               "(F( !p & X(G(h  & m | !h & m & p | h & !m & p | h & p))))\n"
//               "(F(h & m))\n"
//               "(F( !p & X(G( h & m | m & p))))\n"
//               "(F(h & p & X(G(h  & !m | !h & m & p | h ))))\n"
//               "(F(h & m))\n"
//               "(F( !p & X(G( h & m & !p | m & p))))\n"
//               "(F( p & X(G(h  & m | m & p))))\n"
//               "(F(h & p & X(G(h  | m & p))))\n"
//               "(F( h & m))\n"
//               "(F(h & p & X(G(h  & !p | m & p))))\n"
//               "(F( !p & X(G(h  & m | !h & m & p | h & !m & p | h & p))))\n"
//               "(F( p & X(G(h  & m | m & p))))\n"
//               "(F( h & m ))\n"
//               "(F( !p & X(G( h & m | m & p))))\n"
//               "(F( p & X(G(h  & m & !p | !h & m & p | h & p))))\n"
//               "(F(h & p & X(G(h  & !m & !p | !h & m & p | h ))))\n"
//               "(F( h & m ))\n"
//               "(F( !p & X(G( h & m | m & p))))\n"
//               "(F(h & p & X(G(h  | m & p))))\n"
//               "(F(h & m ))\n"
//               "(F( !p & X(G( h & m & !p | m & p))))\n"
//               "(F(h & p & X(G(h  & !p | !h & m & p | h ))))\n"
//               "(F( p & X(G(h  & m | m & p))))\n"
//               "(F(h & m ))\n"
//               "(F( !p & X(G( h & m & !p | m & p))))\n"
//               "(F(h & p & X(G(h  | m & p))))\n"
//               "(F( p & X(G(h  & m | m & p))))");
//   std::cout << abc << std::endl;
//   abc.bc_sort();
// //  std::cout << abc << std::endl;
//   std::cout << "use bc: " << abc.sorted_bcs[0] << std::endl;
//   std::cout << "running model counting..." << std::endl;
//   auto goal = abc.get_fix_goal(3, "/jdk-16.0.2+7/bin/java");
//   std::cout << "get goal: " << goal << std::endl;

  auto domain = LTL::parse("G((p & X(p)) -> X(X(! h)))");
  std::set<LTL> domains;
  domains.insert(domain);
  auto goal = LTL::parse("G(!h | X(p))");
  auto lasso = Lasso(LTL::parse("F(h & m)"));
  FixSolver solver(domains, goal, lasso);
  auto res = solver.next();

  std::cout << std::endl << "Solver Result at level " << 1 << ": " << res.size() << std::endl;
  for(auto& r: res) {
    std::cout << '\t' << r << std::endl;
  }

  res = solver.next();
  std::cout << std::endl << "Solver Result at level " << 2 << ": " << res.size() << std::endl;
  for(auto& r: res) {
    std::cout << '\t' << r << std::endl;
  }
}
