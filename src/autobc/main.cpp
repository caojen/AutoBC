#include "autobc.hpp"

using namespace ltl;
using namespace autobc;

int main() {
  auto d1 = LTL::parse("XXa->b");
  auto d2 = LTL::parse("XX!a->!b");
  auto g1 = LTL::parse("Xa->!b");
  auto g2 = LTL::parse("X!a->b");

  auto abc = AutoBC();
  abc.add_domain(d1);
  abc.add_domain(d2);
  abc.add_goal(g1);
  abc.add_goal(g2);

  std::cout << abc << std::endl;
}
