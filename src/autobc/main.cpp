#include "ltl.hpp"
#include "autobc.hpp"
#include "fix_solver.hpp"
#include "lasso.hpp"
#include "param.hpp"

using namespace ltl;
using namespace autobc;
using namespace param;

int main(int argc, char** argv) {
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
