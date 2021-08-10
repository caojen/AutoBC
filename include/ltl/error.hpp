#pragma once

#include <exception>
#include <string>

namespace ltl {
  class not_a_ltl: public std::exception {
    public:
      virtual char const * what() const noexcept {
        return "Not a valid LTL";
      }
  };

  class unreachable: public std::exception {
    public:
      virtual char const * what() const noexcept {
        return "Code reaches unreachable area";
      }  
  };

  class not_an_integer: public std::exception {
  public:
      virtual char const * what() const noexcept {
        return "BigInteger got a string that is not an integer";
      }
  };
}
