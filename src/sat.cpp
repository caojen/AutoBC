#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdio>

#include "sat.hpp"

namespace ltl {

  static std::string ltl2smv(const std::string& s, const std::set<std::string>& vocab);

  static std::string smvfile(const std::string& s);

  SatSolver * satSolver = nullptr;

  SatSolver::SatSolver(const std::string& path) {
    this->path = path;
  }

  bool SatSolver::operator()(const LTL& ltl) {
    auto iter = cache.find(ltl);
    if(iter != cache.end()) {
      return iter->second;
    } else {
      // 将1设置为a | !a, 0 设置为 a&!a
      dict.set("1", "((a)|(!(a)))");
      dict.set("0", "((a)&(!(a)))");

      auto vocab = dict.get_vocab();

      auto smv = ltl2smv(ltl.serialize(), vocab);
      auto file = SmvFile(smv);
      auto filename = file.sync();

      // 将filename喂给nuXmv
      std::ostringstream cmd("");
      cmd << "/bin/bash -c '" << this->path << " " << filename << " 2>&1'";

      FILE* pipe = popen(cmd.str().c_str(), "r");
      if(!pipe) {
        throw std::runtime_error("popen() failed");
      }
      std::string output;
      char buffer[128] = { 0 };
      while(fgets(buffer, sizeof(buffer), pipe) != NULL) {
        output.append(buffer);
      }
      // 判断ret的某一行中是否存在specification

      std::istringstream ret_stream(output);
      std::string line;

      bool result = false;
      while(std::getline(ret_stream, line)) {
        if(line.find("specification") != std::string::npos) {
          if(line.find("is false") != std::string::npos) {
            result = true;
          }
          break;
        }
      }

      // 恢复字典
      dict.set("1", "1");
      dict.set("0", "0");
      return result;
    }
  }

  std::string ltl2smv(const std::string& s, const std::set<std::string>& vocab) {
    std::ostringstream ostr("");
    ostr << "MODULE main" << std::endl << "VAR" << std::endl;
    for(auto& v: vocab) {
      ostr << v << ":boolean;" << std::endl;
    }
    ostr << "LTLSPEC!(" << std::endl;
    ostr << "(" << s << ")" << std::endl;
    ostr << ")" << std::endl;
    return ostr.str();
  }

  SmvFile::SmvFile(std::string& s) {
    this->content = std::move(s);
  }

  const std::string& SmvFile::sync() {
    std::ostringstream ostr("");
    time_t t = time(nullptr);
    ostr << "tmp-sat-" << t << ".smv";

    this->filename = ostr.str();
    std::ofstream ofstream;
    ofstream.open(this->filename.c_str(), std::ios::out | std::ios::trunc);
    ofstream << this->content;
    ofstream.close();
    return this->filename;
  }

  SmvFile::~SmvFile() {
    remove(this->filename.c_str());
  }
}