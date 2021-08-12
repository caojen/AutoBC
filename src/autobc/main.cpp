#include "ltl.hpp"
#include "autobc.hpp"
#include "fix_solver.hpp"
#include "lasso.hpp"
#include "param.hpp"

using namespace ltl;
using namespace autobc;
using namespace param;

int main(int argc, char** argv) {
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

  // auto domain = LTL::parse("G((p & X(p)) -> X(X(! h)))");
  // std::set<LTL> domains;
  // domains.insert(domain);
  // auto goal = LTL::parse("G(!h | X(p))");
  // auto lasso = Lasso(LTL::parse("F(h & m)"));
  // FixSolver solver(domains, goal, lasso);
  // auto res = solver.next();

  // std::cout << std::endl << "Solver Result at level " << 1 << ": " << res.size() << std::endl;
  // for(auto& r: res) {
  //   std::cout << '\t' << r << std::endl;
  // }

  // res = solver.next();
  // std::cout << std::endl << "Solver Result at level " << 2 << ": " << res.size() << std::endl;
  // for(auto& r: res) {
  //   std::cout << '\t' << r << std::endl;
  // }
  Param parser;
  parser.set("-g", "--global", "global", true, true, "Specify domains and goals File.");
  parser.set("-b", "--bcfile", "bcfile", true, true, "Specify BCs File.");
  parser.set("-i", "--ignore", "ignore", false, false, "If specify, the first line of bcfile will be ignored");
  parser.set("-8", "--jdk1.8", "jdk1.8", true, false, "Specify JDK 1.8 java execute path. Default: /usr/bin/java", "/usr/bin/java");
  parser.set("-6", "--jdk16", "jdk16", true, false, "Specify JDK 16 java execute path. Default: /usr/bin/java", "/usr/bin/java");
  parser.set("-k", "--k", "k", true, true, "Specify K of Model Counting.");
  parser.set("-l", "--level", "level", true, true, "Specify Level of our algorithm.");
  parser.set("-e", "--likelyhood", "likelyhood", true, true, "Specify LikelyHood.jar.");
  parser.set("-m", "--modelcounting", "modelcounting", true, true, "Specify ModelCounting.jar.");

  parser.run(argc, argv);

  FileReader fr;
  auto global = fr(parser["global"]);
  auto bcfile = fr(parser["bcfile"]);
  auto ignore_first_line = parser["ignore"] == "true";
  auto jdk8 = parser["jdk1.8"];
  auto jdk16 = parser["jdk16"];
  auto k = atoi(parser["k"].c_str());
  auto level = atoi(parser["level"].c_str());
  auto likelyhoood = parser["likelyhood"];
  auto modelcounting = parser["modelcounting"];

  auto abc = AutoBC::parse(global);
  abc.likelyhood = likelyhoood;
  abc.modelcounting = modelcounting;
  abc.jdk8 = jdk8;
  abc.use_bcs(bcfile, !ignore_first_line);
  std::cout << abc << std::endl;
  abc.bc_sort();
  auto target_bc = abc.target_bc;
  std::cout << "Target BC is " << *target_bc << std::endl;
  abc.get_fix_goal(k, jdk16);
  auto target_goal = abc.target_goal;
  std::cout << "Target Goal is " << *target_goal << std::endl;

  std::cout << "Fix at level: " << level << std::endl;
  auto fix_result = abc.fix(level);
  std::cout << "Fix Done..." << std::endl;
  for(int i = 0; i < level; i++) {
    std::cout << "Level " << i + 1 << " Fix Result " << std::endl;
    for(auto& goal: fix_result.at(i)) {
      std::cout << "\t" << goal << std::endl;
    }
  }

  return 0;
}
