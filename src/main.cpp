#include <iostream>
#include <fstream>

#include "ltl.hpp"

using namespace autobc;

int main() {
  // (f1 & f2 | f3) & f4 & G (f3)
  {
    auto f1 = Literal("f1");
    auto f2 = Literal("f2");
    auto f3 = Literal("f3");
    auto f4 = Literal("f4");
    LTLGenerator f = LTLGenerator::Gen(f1).aand(f2).oor(f3).aand(f4).aand(LTLGenerator::Gen(f3).global());

    std::cout << LTLGenerator::Gen(f3).global() << std::endl;

    std::cout << f << std::endl;

    std::string input;

    std::cout << "Provide the file, input ltl line by line..." << std::endl;
    std::string path;
    std::cin >> path;

    std::ifstream ifs;
    ifs.open(path);

    while(std::getline(ifs, input)) {
      std::cout << input << std::endl;
      LTL ltl = LTLGenerator::parse(input);
      std::cout << ltl.serialize() << "(result)" <<std::endl;
    }
  }
}
