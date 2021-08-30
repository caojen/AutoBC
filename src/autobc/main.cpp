#include <iomanip>
#include <chrono>

#include "autobc.hpp"
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
    parser.set("-l", "--level", "level", true, false, "Specify Level of our algorithm.");
    parser.set("-e", "--likelyhood", "likelyhood", true, true, "Specify LikelyHood.jar.");
    parser.set("-m", "--modelcounting", "modelcounting", true, true, "Specify ModelCounting.jar.");
    parser.set("-n", "--nuxmv", "nuxmv", true, true, "Specify nuXmv path.");
    parser.set("-t", "--limit", "limit", true, true, "Specify fix results limitation.");

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

    ltl::satSolver = new SatSolver(nuxmv);

    Ranking<ltl::LTL> ranking(modelcounting, jdk16);

    auto abc = AutoBC::parse(global);
    abc.likelyhood = likelyhoood;
    abc.modelcounting = modelcounting;
    abc.jdk8 = jdk8;
    abc.use_bcs(bcfile, !ignore_first_line);
    std::cout << abc << std::endl;
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
    abc.get_fix_goal(k, jdk16);
    auto target_goal = abc.target_goal;
    std::cout << "Target Goal/Domain is " << *target_goal << std::endl;

    std::cout << std::endl;

    // std::cout << "Fix at level: " << level << std::endl;
    // auto const &fix_result = abc.fix(level);
    std::cout << "Fix with limit = " << limit << std::endl;
    auto prev = std::chrono::system_clock::now();
    auto const& fix_result = abc.fix_with_limit(limit);
    auto curr = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> diff = curr - prev;
    std::cout << "Fix Done. (result = " << fix_result.size() << ", time = " << diff.count() <<"ms)" << std::endl;

    std::cout << std::endl;
    std::cout << "Ranking..." << std::endl;
    auto ranked = ranking.rank(abc.domains, abc.goals, *abc.target_goal, fix_result);
    for(auto& ranked_item: ranked) {
        std::cout << std::setw(10) << ranked_item.rank  << " " << ranked_item.item << std::endl;
    }
    

    return 0;

    // C c(3, 5);
    // auto t = c.next();
    // while(!t.empty()) {
    //     std::cout << "(";
    //     for(auto item: t) {
    //         std::cout << item << ", ";
    //     }
    //     std::cout << ")";
    //     std::cout << std::endl;
    //     t = c.next();
    // }
}
