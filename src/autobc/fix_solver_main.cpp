#include <iomanip>
#include <chrono>
#include <iostream>
#include <fstream>

#include "autobc.hpp"
#include "param.hpp"
#include "util.hpp"

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
    parser.set("-l", "--level", "level", true, false, "Specify Level of our algorithm.");
    parser.set("-e", "--likelyhood", "likelyhood", true, true, "Specify LikelyHood.jar.");
    parser.set("-m", "--modelcounting", "modelcounting", true, true, "Specify ModelCounting.jar.");
    parser.set("-n", "--nuxmv", "nuxmv", true, true, "Specify nuXmv path.");
    parser.set("-t", "--limit", "limit", true, true, "Specify fix results limitation.");
    parser.set("-o", "--output", "output", true, true, "Specify result output file.");

    parser.run(argc, argv);

    FileReader fr;
    auto global = fr(parser["global"]);
    auto bcfile = fr(parser["bcfile"]);
    auto ignore_first_line = parser["ignore"] == "true";
    auto jdk8 = parser["jdk1.8"];
    auto jdk16 = parser["jdk16"];
    auto k = atoi(parser["k"].c_str());
    // auto level = atoi(parser["level"].c_str());
    auto likelyhoood = parser["likelyhood"];
    auto modelcounting = parser["modelcounting"];
    auto nuxmv = parser["nuxmv"];
    auto limit = atoi(parser["limit"].c_str());
    auto output = parser["output"];

    ltl::satSolver = new SatSolver(nuxmv);

    auto abc = AutoBC::parse(global);
    abc.likelyhood = likelyhoood;
    abc.modelcounting = modelcounting;
    abc.jdk8 = jdk8;
    abc.use_bcs(bcfile, !ignore_first_line);
    std::cout << abc << std::endl;

    std::cout << "Finding Target BC..." << std::endl;
    if(abc.bcs.size() == 0) {
        std::cerr << "No Bc Found. Abort" << std::endl;
        abort();
    }
    abc.bc_sort();
    auto target_bc = abc.target_bc;
    std::cout << "Target BC is " << *target_bc << std::endl;
    {
        Lasso lasso = Lasso(*target_bc);
        auto terms = lasso.fetch_terms(1);
        std::cout << "-- Terms:" << std::endl;
        for(auto& term: terms) {
            std::cout << "-- -- " << term << std::endl;
        }
    }
    std::cout << "Finding Target Goal..." << std::endl;
    abc.get_fix_goal(k, jdk16);
    auto target_goal = abc.target_goal;

    std::cout << "Target Goal/Domain is " << *target_goal << std::endl;

    std::cout << std::endl;

    std::set<ltl::LTL> ref_result;
    std::set<ltl::LTL> random_result;
    {

        std::cout << "Fix with limit = " << limit << std::endl;
        auto prev = std::chrono::system_clock::now();
        auto const& fix_result = abc.fix_with_limit(limit);
        ref_result = fix_result;

        auto curr = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> diff = curr - prev;
        std::cout << "Fix Done. (result = " << fix_result.size() << ", time = " << diff.count() <<"ms)" << std::endl;

        std::cout << std::endl;
        std::cout << "Will Print Fix_Result Into Outfile..." << std::endl;

        std::ofstream ofstream(output);

        for(auto& domain: abc.domains) {
          ofstream << "domain=" << domain << std::endl;
        }
        for(auto& goal: abc.goals) {
          if(&goal != abc.target_goal) {
            ofstream << "goal=" << goal << std::endl;
          }
        }
        ofstream << "target=" << *abc.target_goal << std::endl;
        for(auto& item: fix_result) {
            ofstream << "fixed=" << item << std::endl;
        }

        std::cout << "Ref Fix Done." << std::endl;
    }
    

    return 0;
}
