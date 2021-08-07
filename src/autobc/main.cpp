#include "autobc.hpp"

using namespace ltl;
using namespace autobc;

int main() {
  format_as_symbol = true;

  std::string content = "Domains: [] (q -> s)\n"
                        "Goals: [] (r -> [](!s)), [] (p -> <>(q))";
  auto abc2 = AutoBC::parse(content);
  std::cout << abc2 << std::endl;
}
