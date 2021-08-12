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
        this->terms.insert({ true, node->literal });
      } else if(node->is_op1()) {
        if(
          dynamic_cast<Not*>(node->op.get()) != nullptr &&
          node->right->is_literal()
        ) {
          this->terms.insert({ false, node->right->literal });
        } else {
          nodes.push(node->right);
        }
      } else if(node->is_op2()) {
        nodes.push(node->left);
        nodes.push(node->right);
      } else {
        throw not_a_ltl();
      }
    }

    this->always_true = false;
    for(auto& term: this->terms) {
      for(auto& i: this->terms) {
        if(term.first != i.first && term.second == i.second) {
          this->always_true = true;
          break;
        }
      }
      if(this->always_true) {
        break;
      }
    }


    if(!this->always_true) {
      std::string str;
      for(auto iter = this->terms.begin(); iter != this->terms.end(); ++iter) {
        if(!iter->first) {
          str.append("!");
        }
        str.append(iter->second.get()->serialize());
        if(std::next(iter) != this->terms.end()) {
          str.append("&");
        }
      }
      this->to = LTL::parse(str);
    }
  }
}
