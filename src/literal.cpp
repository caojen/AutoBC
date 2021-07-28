#include "literal.hpp"

namespace autobc {
  Literal::Literal() {}

  Literal::Literal(const char& ch) {
    this->s = "";
    this->s += ch;
  }

  Literal::Literal(const char* str) {
    this->s = std::string(str);
  }

  Literal::Literal(const std::string& str) {
    this->s = str;
  }

  Literal::Literal(const Literal& other) {
    this->s = other.s;
  }

  bool Literal::empty() const {
    return this->s == "";
  }
 
  bool Literal::operator==(const Literal& other) const {
    return this->s == other.s;
  }

  bool Literal::operator!=(const Literal& other) const {
    return this->s != other.s;
  }

  std::ostream& operator<<(std::ostream& o, const Literal& li) {
    o << li.s;
    return o;
  }
}
