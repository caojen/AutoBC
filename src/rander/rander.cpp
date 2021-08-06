#include <fstream>
#include <iostream>

#include "ltl/ltl.hpp"
#include "ltl/sat.hpp"
#include "rander/rnd.hpp"

using namespace ltl;
using namespace rnd;

void usage(int argc, char** argv);

int main(int argc, char** argv) {

  if(argc < 8) {
    usage(argc, argv);
    abort();
  }

  auto start    = std::chrono::system_clock::now();

  std::string   nuXmv(argv[1]);
  unsigned      target = atoi(argv[2]);
  unsigned      fdepth = atoi(argv[3]);
  unsigned      mdepth = atoi(argv[4]);
  unsigned      stime  = atoi(argv[5]);
  unsigned      vocabLen = atoi(argv[6]);
  std::string   ofilename(argv[7]);

  std::ofstream ofstream;
  ofstream.open(ofilename, std::ios::trunc | std::ios::out);

  std::srand(std::time(nullptr));
  satSolver = new SatSolver(nuXmv);

  Rander& rander = *(new RanderCNF(std::chrono::seconds(stime), vocabLen));

  unsigned solved = 0;
  unsigned loop = 0;

  while(true) {
    auto res = rander(target, Range(fdepth, mdepth));
    for(auto& pair: res) {
      ++solved;
      ofstream << pair.first << std::endl;
      ofstream << pair.second << std::endl;
    }
    ++loop;
    std::cout << "Loop " << loop << " Found " << res.size() << " Total: " << solved << "/" << target << " ";
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << "[" << duration.count() << "s]" << std::endl;
    if(solved >= target) {
      break;
    }
  }

  ofstream.close();
}

void usage(int argc, char** argv) {
  std::cout << "Usage: " << argv[0] << " $1 $2 $3 $4 $5 $6 $7" << std::endl << std::endl;
  std::cout << "- 程序说明：" << std::endl;
  std::cout << "- 程序将会自动生成多对不同的LTL公式，每一对中包含一个SAT和一个UnSAT" << std::endl;
  std::cout << "- 程序算法：" << std::endl;
  std::cout << "- 每次从根开始构建LTL公式，每次构建不会超过$4秒，树高度不超过$3，构建的LTL公式最少需要3层树高" << std::endl;
  std::cout << "- 使用$1所指定的nuXmv程序来进行公式的SAT-UnSAT判定" << std::endl;
  std::cout << "- 参数列表:" << std::endl;
  std::cout << "- $1: 指明nuXmv的路径，例如: /usr/local/bin/nuXmv" << std::endl;
  std::cout << "      项目中已经包含了linux版本的nuXmv，存放在./bin中。其他版本请到官网下载" << std::endl;
  std::cout << "- $2: 指明需要生成的LTL公式的数量，例如 10000 代表生成 10000 对" << std::endl;
  std::cout << "- $3: 指明所生成的LTL公式的最低深度，例如 5" << std::endl;
  std::cout << "- $4: 指明所生成的LTL公式的最高深度，例如 20" << std::endl;
  std::cout << "- $5: 指明每次搜索时间（秒），例如 20" << std::endl;
  std::cout << "- $6: 指明字典中的变量数。变量从小写b开始，到小写z结束。例如 3 代表可用的变量数为 b, c, d" << std::endl;
  std::cout << "- $7: 指明最终输出的结果存放位置。生成的LTL公式会逐步添加到该文件中，如果该文件已经存在，那么重新创建该文件" << std::endl;
  std::cout << "-     其中，文件中奇数行是SAT的LTL公式，偶数行则是对应的UnSAT公式" << std::endl;
  std::cout << std::endl;
  std::cout << "- 例子：" << std::endl;
  std::cout << "- " << argv[0] << " /usr/local/bin/nuXmv 10000 50 100 20 3 ./output.txt" << std::endl;
  std::cout << "- " << argv[0] << " ../bin/nuXmv-Linux-2.0.0 30000 15 30 15 15 ./output.txt" << std::endl;
}
