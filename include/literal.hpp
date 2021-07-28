#pragma once

#include <string>

namespace autobc {
  class Literal {
    public:
      Literal();
      Literal(const char& ch);
      Literal(const std::string& str);
      Literal(const Literal& other);

      bool operator==(const Literal& other) const;
      bool operator!=(const Literal& other) const;
    private:
      std::string s = "";
  };
}
