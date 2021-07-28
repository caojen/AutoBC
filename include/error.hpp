#include <exception>
#include <string>

namespace autobc {
  class not_enough_parameter: public std::exception {
    public:
      not_enough_parameter(std::string& op, unsigned expected, unsigned got) {
        this->op = op;
        this->expected = expected;
        this->got = got;
      }
      char const * what() const {
        std::string s;
        s += "操作符" + s + "期望操作数数量为: " + std::to_string(expected);
        s += "但得到了: " + std::to_string(got) + "个操作数";

        return s.c_str(); 
      }
    private:
      std::string   op;
      unsigned      expected;
      unsigned      got;
  };

  class not_a_ltl: public std::exception {
    public:
      char const * what() const {
        return "程序逻辑错误，似乎对一个命题逻辑语句调用了LTL的时态算子";
      }
  };
}
