#include <iomanip>
#include <chrono>

#include "autobc.hpp"
#include "param.hpp"
#include "util.hpp"
#include "random_solver.hpp"
#include "ranking.hpp"

using namespace ltl;
using namespace autobc;
using namespace param;

bool use_model = false;

int main(int argc, char** argv) {
    Param parser;
    parser.set("-i", "--ignore", "ignore", false, false, "If specify, the first line of bcfile will be ignored");
    parser.set("-8", "--jdk1.8", "jdk1.8", true, false, "Specify JDK 1.8 java execute path. Default: /usr/bin/java", "/usr/bin/java");
    parser.set("-6", "--jdk16", "jdk16", true, false, "Specify JDK 16 java execute path. Default: /usr/bin/java", "/usr/bin/java");
    parser.set("-k", "--k", "k", true, true, "Specify K of Model Counting.");
    parser.set("-l", "--level", "level", true, false, "Specify Level of our algorithm.");
    parser.set("-e", "--likelyhood", "likelyhood", true, true, "Specify LikelyHood.jar.");
    parser.set("-m", "--modelcounting", "modelcounting", true, true, "Specify ModelCounting.jar.");
    parser.set("-n", "--nuxmv", "nuxmv", true, true, "Specify nuXmv path.");
    parser.set("-t", "--limit", "limit", true, true, "Specify fix results limitation.");
    parser.set("-r", "--random", "random", true, true, "Specify random_solver output file.");
    parser.set("-f", "--ref", "ref", true, true, "Specify ref_solver output file.");
    parser.set("-d", "--model", "model", false, false, "If specify, use model_ltl when synSim");

    parser.run(argc, argv);

    FileReader fr;
    auto ignore_first_line = parser["ignore"] == "true";
    auto jdk8 = parser["jdk1.8"];
    auto jdk16 = parser["jdk16"];
    auto k = atoi(parser["k"].c_str());
    // auto level = atoi(parser["level"].c_str());
    auto likelyhoood = parser["likelyhood"];
    auto modelcounting = parser["modelcounting"];
    auto nuxmv = parser["nuxmv"];
    auto limit = atoi(parser["limit"].c_str());
    auto random_file = parser["random"];
    auto ref_file = parser["ref"];
    use_model = parser["model"] == "true";

    auto random_file_content = fr(random_file);
    auto ref_file_content = fr(ref_file);

    ltl::satSolver = new SatSolver(nuxmv);

    Ranking ranking(modelcounting, jdk16, k);

    auto random_result = OriginResult::parse(random_file_content);
    auto ref_result = OriginResult::parse(ref_file_content);

    std::cout << "Ranking Ref Results: " << ref_result.replacements.size() << std::endl;
    auto ref_rank_result = ranking.rank(ref_result);

    std::cout << "Ref Solver Results:" << std::endl;
    std::cout << std::endl << std::setw(10) << "semSim"
            << std::setw(10) << "synSim"
            << std::setw(10) << "len_diff" << "\t" << "ltl"
            << std::endl;
    for(auto &r: ref_rank_result) {
        std::cout << std::setw(10) << r.rank
            << std::setw(10) << r.syn
            << std::setw(10) << r.length << "\t" << r.ltl
            << std::endl;
    }

    std::cout << "Ranking Random Results: " << random_result.replacements.size() << std::endl;
    auto random_rank_result = ranking.rank(random_result);

    std::cout << "Random Solver Results:" << std::endl;
    std::cout << std::endl << std::setw(10) << "semSim"
            << std::setw(10) << "synSim"
            << std::setw(10) << "len_diff" << "\t" << "ltl"
            << std::endl;
    for(auto &r: random_rank_result) {
        std::cout << std::setw(10) << r.rank
            << std::setw(10) << r.syn
            << std::setw(10) << r.length << "\t" << r.ltl
            << std::endl;
    }
    std::cout << std::endl;

    std::set<RankResultItem> rank_set;
    for(auto& r: random_rank_result) {
        rank_set.insert(r);
    }
    for(auto& r: ref_rank_result) {
        rank_set.insert(r);
    }

    const unsigned use_top = 5;
    auto top = get_top<use_top>(rank_set);

    unsigned formula_count = 0;

    std::cout << "Top " <<  use_top << ":" << std::endl;
    for(unsigned i = 0; i < use_top; i++) {
        std::cout << " Top " << i + 1 << ":" << std::endl;
        for(auto& r: top[i]) {
            ++formula_count;
            std::cout << std::setw(10) << r.rank
                << std::setw(10) << r.syn
                << std::setw(10) << r.length << "\t" << r.ltl
                << std::endl;
        }
    }

    std::cout << std::endl;
    std::cout << "Formula Count = " << formula_count << std::endl;

    unsigned ref_count = 0;
    unsigned random_count = 0;

    for(auto& f: ref_rank_result) {
        if(in_top<use_top>(top, f.ltl)) {
            ++ref_count;
        }
    }

    for(auto& f: random_rank_result) {
        if(in_top<use_top>(top, f.ltl)) {
            ++random_count;
        }
    }

    std::cout << "Ref Solver Count    = " << ref_count << "   " << static_cast<double>(ref_count) / static_cast<double>(formula_count) << std::endl;
    std::cout << "Random Solver Count = " << random_count << "   " << static_cast<double>(random_count) / static_cast<double>(formula_count) << std::endl;

    return 0;
}
