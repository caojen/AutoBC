// 属于LTL中序表达式，转换成后序表达式

#include <iostream>

#include "ltl.hpp"

int main(int argc, char** argv) {
  if(argc != 2) {
    std::cout << "Usage: " << argv[0] << " ltl" << std::endl;
    std::cout << "e.g  : " << argv[0] << " X(X(a))" << std::endl;
    return 1;
  }

  std::string l = argv[1];
  auto inorder = ltl::split_into_in_order(l);
  auto preorder = ltl::in_order_to_post_order(inorder);
  for(auto& p: preorder) {
    std::cout << p;
  }
  std::cout << std::endl;
}
