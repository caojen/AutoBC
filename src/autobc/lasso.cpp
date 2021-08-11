#include <queue>

#include "lasso.hpp"

using namespace ltl;

typedef LTL::LTLNode LTLNode;

namespace autobc {
  Lasso::Lasso(const LTL& ltl) {
    std::queue<std::shared_ptr<LTLNode>> nodes;
    nodes.push(ltl.root);

    while(!nodes.empty()) {
      auto node = nodes.front(); nodes.pop();

      if(node->is_literal()) {
        this->terms.insert(node->literal);
      } else if(node->is_op1()) {
        nodes.push(node->right);
      } else if(node->is_op2()) {
        nodes.push(node->left);
        nodes.push(node->right);
      } else {
        throw not_a_ltl();
      }
    }

    std::string str;
    for(auto iter = terms.begin(); iter != terms.end(); ++iter) {
      str.append((*iter)->serialize());
      if(std::next(iter) != terms.end()) {
        str.append("&");
      }
    }

    this->to = LTL::parse(str);
  }
}
