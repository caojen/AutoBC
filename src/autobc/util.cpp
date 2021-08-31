#include "util.hpp"

using namespace ltl;

namespace autobc {

  unsigned sf(const ltl::LTL& formula) {
    return sf(formula.root);
  }

  unsigned sf(const std::shared_ptr<ltl::LTL::LTLNode>& root) {
    if(root->is_literal()) {
      return 1;
    } else if(root->is_op1()) {
      return 1 + sf(root->right);
    } else if(root->is_op2()) {
      return 1 + sf(root->left) + sf(root->right);
    }
  }

}
