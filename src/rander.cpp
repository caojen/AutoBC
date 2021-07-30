#include <fstream>
#include <iostream>

#include "ltl.hpp"
#include "sat.hpp"
#include "rnd.hpp"

using namespace ltl;
using namespace rnd;

void usage(int argc, char** argv);

int main(int argc, char** argv) {
  if(argc < 6) {
    usage(argc, argv);
    abort();
  }

  std::string   nuXmv(argv[1]);
  unsigned      target = atoi(argv[2]);
  unsigned      mdepth = atoi(argv[3]);
  unsigned      stime  = atoi(argv[4]);
  std::string   ofilename(argv[5]);

  std::ofstream ofstream;
  ofstream.open(ofilename, std::ios::trunc | std::ios::out);

  std::srand(std::time(nullptr));
  satSolver = new SatSolver(nuXmv);

  Rander& rander = *(new RanderCNF(std::chrono::seconds(stime)));

  unsigned solved = 0;
  unsigned loop = 0;

  while(true) {
    auto res = rander(target, Range(3, mdepth));
    for(auto& pair: res) {
      ++solved;
      ofstream << pair.first << std::endl;
      ofstream << pair.second << std::endl;
    }
    ++loop;
    std::cout << "Loop " << loop << " Found " << res.size() << " Total: " << solved << "/" << target << std::endl;
    if(solved >= target) {
      break;
    }
  }

  ofstream.close();
}

void usage(int argc, char** argv) {
  std::cout << "Usage: " <<  argv[0] << " {nuXmv-absolute-path} {# target} {# max_depth} {# search_time} {output-file-path}" << std::endl;
  std::cout << "例如：" << argv[0] << " /usr/local/bin/nuXmv 10000 100 20 ./output" << std::endl;
  std::cout << "表示最终生成100对sat-unsat公式，树最深长度为100（最浅为3）" <<
    "，每次搜索最多20s（以避免树无限增长）。每次搜索完成后，如果还没达到100对公式的目标，那么重新搜索" << std::endl;
  std::cout << "每次搜索完后，结果就会追加并保存到新文件./output（会截断已有文件内容）" << std::endl;
}
