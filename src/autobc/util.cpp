#include <algorithm>
#include "util.hpp"

using namespace ltl;

namespace autobc {

  std::set<ltl::LTL> sf(const ltl::LTL& formula) {
    return sf(formula.root);
  }

  std::set<ltl::LTL> sf(const std::shared_ptr<ltl::LTL::LTLNode>& root) {
    std::set<ltl::LTL> ret;
    if(root->is_literal()) {
      ret.insert(LTL(*root->literal));
    } else if(root->is_op1()) {
      ret.insert(LTL(*root));
      auto s = sf(root->right);
      for(auto& ss: s) {
        ret.insert(ss);
      }
    } else if(root->is_op2()) {
      ret.insert(LTL(*root));
      auto s = sf(root->right);
      for(auto& ss: s) {
        ret.insert(ss);
      }
      s = sf(root->left);
      for(auto& ss: s) {
        ret.insert(ss);
      }
    }

    return ret;
  }

  double synSim(const ltl::LTL& f1, const ltl::LTL& f2, bool use_model) {
    auto sf1 = sf(f1);
    auto sf2 = sf(f2);
    if(use_model) {
      sf1 = sf(model_ltl(f1));
      sf2 = sf(model_ltl(f2));
    }

    std::set<ltl::LTL> intersection;
    std::set_intersection(sf1.begin(), sf1.end(), sf2.begin(), sf2.end(), std::inserter(intersection, intersection.begin()));

    double a = static_cast<double>(intersection.size());
    return 0.5 * (a / sf1.size() + a / sf2.size());
  }

  ltl::LTL model_ltl(const ltl::LTL& f) {
    ltl::LTL r = f;
    to_model(r.root);
    return r;
  }

  void to_model(std::shared_ptr<ltl::LTL::LTLNode>& root) {
    if(root->is_boolean_formula()) {
      root = std::make_shared<ltl::LTL::LTLNode>(*dict.get("fffffffffffff"));
    } else {
      if(root->is_op1()) {
        to_model(root->right);
      } else if(root->is_op2()) {
        to_model(root->left);
        to_model(root->right);
      }
    }
  }
}
