#pragma once

#include <fstream>
#include <iostream>

namespace param {
  class FileReader {
    public:
      FileReader() = default;
      std::string operator()(const std::string& filename) const {
        std::ifstream ifstream;
        ifstream.open(filename);
        std::string ret;
        std::string line;
        while(std::getline(ifstream, line)) {
          ret.append(line + "\n");
        }
        return ret;
      }
  };
}
