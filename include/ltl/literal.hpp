#pragma once

#include <string>
#include <iostream>

namespace ltl {
  class Literal {
    public:
      Literal();
      Literal(const char& ch);
      Literal(const char* str);
      Literal(const std::string& str);
      Literal(const Literal& other);

      void set(const Literal& other);

      bool empty() const;
      bool operator==(const Literal& other) const;
      bool operator!=(const Literal& other) const;
      
      friend std::ostream& operator<<(std::ostream& o, const Literal& li);
      std::string serialize() const;

    private:
      std::string s = "";
  };
}
