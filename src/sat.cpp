#include <ctime>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <thread>
#include <cerrno>
#include <cstring>

#include "sat.hpp"

namespace ltl {

  static std::string ltl2smv(const std::string& s, const std::set<std::string>& vocab);

  SatSolver * satSolver = nullptr;

  SatSolver::SatSolver(const std::string& path) {
    this->path = path;
    const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    const int len = 16;
    this->solverName = "";
    for(int i = 0; i < len; i++) {
      this->solverName.append(std::string(1, charset[std::rand() % sizeof(charset)]));
    }
    std::cout << "SolverName: " << this->solverName << std::endl;
  }

  bool SatSolver::operator()(const LTL& ltl) {
      // 将1设置为a | !a, 0 设置为 a&!a
      dict.set("1", "((a)|(!(a)))");
      dict.set("0", "((a)&(!(a)))");

      auto str_ltl = ltl.serialize();
      auto iter = this->cache.find(str_ltl);
      if(iter != this->cache.end()) {
        return iter->second;
      }

      auto vocab = dict.get_vocab();

      auto smv = ltl2smv(str_ltl, vocab);
      auto file = SmvFile(this->solverName, std::to_string(this->idx), smv);
      ++this->idx;
      auto filename = file.sync();
      bool result = false;

      feed_nuXmv:
      try {
        // 将filename喂给nuXmv
        std::ostringstream cmd("");
        cmd << this->path << " " << filename << " 2>&1";
        std::cout << cmd.str() << std::endl;
        FILE* pipe = popen(cmd.str().c_str(), "r");
        if(!pipe) {
          throw std::runtime_error(strerror(errno));
        }
        std::string output;
        char buffer[10240] = { 0 };
        while(fgets(buffer, sizeof(buffer), pipe) != NULL) {
          output.append(buffer);
        }
        cmd << "done" << std::endl;
        pclose(pipe);
        // 判断ret的某一行中是否存在specification

        std::istringstream ret_stream(output);
        std::string line;

        while(std::getline(ret_stream, line)) {
          if(line.find("specification") != std::string::npos) {
            if(line.find("is false") != std::string::npos) {
              result = true;
            } else if(line.find("is true") != std::string::npos) {
              result = false;
            } else {
              // unreachable.
              std::cout << "Fatal: SatSolver catches specification, but no 'is true' or 'is false' exists. Abort." << std::endl;
              std::cout << "Fatal: Line: " << line << std::endl;
              abort();
            }
            break;
          }
        }
      } catch(std::runtime_error& re) {
        std::cout << "SatSolver Popen Error!" << std::endl;
        std::cout << re.what() << std::endl;
        std::cout << "Sleep 1s and Retry..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        goto feed_nuXmv;
      }

      this->cache[str_ltl] = result;
      // 恢复字典
      dict.set("1", "1");
      dict.set("0", "0");
      return result;
  }

  bool SatSolver::checkSAT(const LTL &ltl) {
    return (*this)(ltl);
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

  SmvFile::SmvFile(const std::string& prefix, const std::string& append, std::string& s) {
    this->prefix = prefix;
    this->append = append;
    this->content = std::move(s);
  }

  const std::string& SmvFile::sync() {
    std::ostringstream ostr("");
    time_t t = time(nullptr);
    ostr << "tmp-sat-" << this->prefix << "-" << t << this->append << ".smv";

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
