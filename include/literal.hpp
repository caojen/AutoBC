#pragma once

#include <string>
#include <iostream>

namespace autobc {
  class Literal {
    public:
      Literal();
      explicit Literal(const char& ch);
      explicit Literal(const std::string& str);
      Literal(const Literal& other);

      bool operator==(const Literal& other) const;
      bool operator!=(const Literal& other) const;
      
      friend std::ostream& operator<<(std::ostream& o, const Literal& li);

    private:
      std::string s = "";
  };
}
