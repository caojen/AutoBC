#include <exception>
#include <string>

namespace ltl {
  class not_a_ltl: public std::exception {
    public:
      char const * what() const noexcept {
        return "Not a valid LTL";
      }
  };

  class unreachable: public std::exception {
    public:
      char const * what() const noexcept {
        return "Code reaches unreachable area";
      }  
  };
}
