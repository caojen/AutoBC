#pragma once

#include <map>

#include "ltl.hpp"

namespace ltl {
  class LTL;
  class SatSolver {
    public:
      SatSolver(const std::string& path = "/usr/local/bin/nuXmv");

      bool operator()(const LTL& ltl);

      bool checkSAT(const LTL& ltl);

    private:
      std::string         path;
      std::string         solverName;
      int                 idx = 0;
      std::map<std::string, bool>  cache;
  };

  extern SatSolver* satSolver;

  class SmvFile {
  public:
      SmvFile() = default;

      // 将s移动到本身
      SmvFile(const std::string& prefix, const std::string& append, std::string& s);

      // 将content保存到文件中，返回文件名
      const std::string& sync();

      // 删除文件
      ~SmvFile();
  private:
      std::string content;
      std::string filename;
      std::string prefix;
      std::string append;
  };
}
