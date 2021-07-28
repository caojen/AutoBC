#include <exception>
#include <string>

namespace autobc {
  class not_a_ltl: public std::exception {
    public:
      char const * what() const noexcept {
        return "Not a valid LTL";
      }
  };
}
