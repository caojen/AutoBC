#include "fix_solver.hpp"

using namespace ltl;

static auto get_all_literals = [](const ltl::LTL& f) -> std::set<std::pair<bool, std::shared_ptr<ltl::Literal>>> {
  std::set<std::pair<bool, std::shared_ptr<ltl::Literal>>> ret;
  std::queue<std::shared_ptr<ltl::LTL::LTLNode>> queue;
  if(f.root.get() == nullptr) {
    return ret;
  } else {
    queue.push(f.root);
  }

  while(!queue.empty()) {
    auto node = queue.front(); queue.pop();
    if(node->is_literal()) {
      ret.insert({ true, node->literal });
    } else if(node->is_op1()) {
      if(dynamic_cast<Not*>(node->op.get()) != nullptr && node->right->is_literal()) {
        ret.insert({ false, node->right->literal });
      } else {
        queue.push(node->right);
      }
    } else if(node->is_op2()) {
      queue.push(node->left);
      queue.push(node->right);
    } else {
      throw not_a_ltl();
    }
  }

  return ret;
};

namespace autobc {
  FixSolver::FixSolver(const std::set<ltl::LTL>& domains, const ltl::LTL& goal, const Lasso& bc) {
    this->domains = domains;
    this->goal = goal;
    this->bc = bc;
    this->k = 0;

    this->prev.insert(goal);
    this->used.insert(goal);
  }

  const std::set<LTL>& FixSolver::next() {
    this->k++;
    std::set<ltl::LTL> next;

    for(auto& formula: this->prev) {
      auto wrs = WR(formula, bc);
      auto srs = SR(formula, bc);

      for(auto& wr: wrs) {
        if(used.find(wr) == used.end()) {
          used.insert(wr);
          next.insert(wr);
        }
      }

      for(auto& sr: srs) {
        if(used.find(sr) == used.end()) {
          used.insert(sr);
          next.insert(sr);
        }
      }
    }

    this->prev = std::move(next);
    return this->prev;
  }

  std::set<LTL> FixSolver::SR(const LTL& formula, const Lasso& lasso) {
    std::set<LTL> ret;

    auto& root = formula.root;

    // SR(f) = 

    // 1: G(f)
    ret.emplace(formula.global());
    // 3. if f = f1 | f2
    if(root->op == op::oor) {
      // f1:
      ret.emplace(root->left);
      // f2:
      ret.emplace(root->right);
      // f2 R f1
      auto f1 = LTL(root->right);
      auto f2 = LTL(root->left);
      ret.emplace(f2.release(f1));
      // f2 U f1
      ret.emplace(f2.until(f1));
      // f1 R f2
      ret.emplace(f1.release(f2));
      // f1 U f2
      ret.emplace(f1.until(f2));
      // f1 | f2', for f2' in SR(f2)
      auto f2_dots = SR(f2, lasso);
      for(auto &f2_dot: f2_dots) {
        ret.emplace(f1.oor(f2_dot));
      }
      // f1' | f2, for f1' in SR(f1)
      auto f1_dots = SR(f1, lasso);
      for(auto &f1_dot: f1_dots) {
        ret.emplace(f1_dot.oor(f2));
      }
    }
    
    // 4. if f = f1 & f2
    else if(root->op == op::aand) {
      // f1' | f2, for f1' in SR(f1)
      auto f1 = LTL(root->right);
      auto f2 = LTL(root->left);
      auto f1_dots = SR(f1, lasso);
      for(auto& f1_dot: f1_dots) {
        ret.emplace(f1_dot.oor(f2));
      }
      auto f2_dots = SR(f2, lasso);
      for(auto& f2_dot: f2_dots) {
        ret.emplace(f1.oor(f2_dot));
      }
    }

    // 5. if f = X f1
    else if(root->op == op::next) {
      // X f1' for f1' in SR(f1)
      auto f1 = LTL(root->right);
      auto f1_dots = SR(f1, lasso);
      for(auto& f1_dot: f1_dots) {
        ret.emplace(f1_dot.next());
      }
    }

    // 6. if f = F f1
    else if(root->op == op::finally) {
      // F f1' for f1' in SR(f1)
      auto f1 = LTL(root->right);
      auto f1_dots = SR(f1, lasso);
      for(auto& f1_dot: f1_dots) {
        ret.emplace(f1_dot.finally());
      }
      // G f1
      ret.emplace(f1.global());
      // X f1
      ret.emplace(f1.next());
      // f1
      ret.emplace(f1);
    }

    // 7. if f = G f1
    else if(root->op == op::global) {
      auto f1 = LTL(root->right);
      // G f1' for f1' in SR(f1)
      auto f1_dots = SR(f1, lasso);
      for(auto& f1_dot: f1_dots) {
        ret.emplace(f1_dot.global());
      }
    }

    // 8. if f = f1 U f2
    else if(root->op == op::until) {
      auto f1 = LTL(root->right);
      auto f2 = LTL(root->left);
      // f1' U f2, for f1' in SR(f1)
      auto f1_dots = SR(f1, lasso);
      for(auto& f1_dot: f1_dots) {
        ret.emplace(f1_dot.until(f2));
      }
      // f1 U f2', for f2' in SR(f2)
      auto f2_dots = SR(f2, lasso);
      for(auto& f2_dot: f2_dots) {
        ret.emplace(f1.until(f2_dot));
      }
      // f1 & f2
      ret.emplace(f1.aand(f2));
      // f2
      ret.emplace(f2);
    }

    // 9. if f = f1 R f2
    else if(root->op == op::release) {
      auto f1 = LTL(root->right);
      auto f2 = LTL(root->left);
      // f1' R f2, for f1' in SR(f1)
      auto f1_dots = SR(f1, lasso);
      for(auto& f1_dot: f1_dots) {
        ret.emplace(f1_dot.release(f2));
      }
      // f1 R f2', for f2' in SR(f2)
      auto f2_dots = SR(f2, lasso);
      for(auto& f2_dot: f2_dots) {
        ret.emplace(f1.release(f2_dot));
      }
    }

    // 2. 选择在formula出现的可以蕴含lasso的状态去除，合取term的非
    if(!lasso.always_false && formula.is_boolean_formula()) {
      auto all_literals = get_all_literals(formula);
      bool exists = false;

      for(auto &literal: all_literals) {
        for(auto& term: lasso.terms) {
          if(literal.first != term.first && literal.second == term.second) {
            exists = true;
            break;
          }
        }
      }
      if(!exists) {
        ret.emplace(formula.aand(lasso.to.nnot()));
      }
    }
    return ret;
  }

  std::set<LTL> FixSolver::WR(const LTL& formula, const Lasso& lasso) {
    std::set<LTL> ret;

    auto& root = formula.root;

    // WR(f) = 
    // 1. F f
    ret.emplace(formula.finally());

    // 3. if f = f1 | f2
    if(root->op == op::oor) {
      auto f1 = LTL(root->left);
      auto f2 = LTL(root->right);
      // f1' | f2, for f1' in WR(f1)
      auto f1_dots = WR(f1, lasso);
      for(auto& f1_dot: f1_dots) {
        ret.emplace(f1_dot.oor(f2));
      }
      // f1 | f2' for f2' in WR(f2)
      auto f2_dots = WR(f2, lasso);
      for(auto& f2_dot: f2_dots) {
        ret.emplace(f1.oor(f2_dot));
      }
    } 

    // 4. f = f1 & f2
    else if(root->op == op::aand) {
      auto f1 = LTL(root->left);
      auto f2 = LTL(root->right);
      // f1' | f2, for f1' in WR(f1)
      auto f1_dots = WR(f1, lasso);
      for(auto& f1_dot: f1_dots) {
        ret.emplace(f1_dot.oor(f2));
      }
      // f1 | f2' for f2' in WR(f2)
      auto f2_dots = WR(f2, lasso);
      for(auto& f2_dot: f2_dots) {
        ret.emplace(f1.oor(f2_dot));
      }

      // f1 U f2
      ret.emplace(f1.until(f2));
      // f1
      ret.emplace(f1);
      // f2
      ret.emplace(f2);
    }

    // 5. f = X f1
    else if(root->op == op::next) {
      auto f1 = LTL(root->right);
      // X f1', for f1' in WR(f1)
      auto f1_dots = WR(f1, lasso);
      for(auto& f1_dot: f1_dots) {
        ret.emplace(f1_dot.next());
      }
      // F f1
      ret.emplace(f1.finally());
    }

    // 6. F f1
    else if(root->op == op::finally) {
      // F f1' for f1' in WR(f1)
      auto f1 = LTL(root->right);
      auto f1_dots = WR(f1, lasso);
      for(auto& f1_dot: f1_dots) {
        ret.emplace(f1_dot.finally());
      }
    }

    // 7. G f1
    else if(root->op == op::global) {
      // G f1' for f1' in WR(f1)
      auto f1 = LTL(root->right);
      auto f1_dots = WR(f1, lasso);
      for(auto& f1_dot: f1_dots) {
        ret.emplace(f1_dot.global());
      }
      // F f1
      ret.emplace(f1.finally());
      // f1
      ret.emplace(f1);
    }

    // 8. f1 U f2
    else if(root->op == op::until) {
      auto f1 = LTL(root->left);
      auto f2 = LTL(root->right);
      // f1' U f2, for f1' in WR(f1)
      auto f1_dots = WR(f1, lasso);
      for(auto& f1_dot: f1_dots) {
        ret.emplace(f1_dot.until(f2));
      }
      // f1 U f2' for f2' in WR(f2)
      auto f2_dots = WR(f2, lasso);
      for(auto& f2_dot: f2_dots) {
        ret.emplace(f1.until(f2_dot));
      }
      // f1 | f2
      ret.emplace(f1.oor(f2));
    }

    // 9. f1 R f2
    else if(root->op == op::release) {
      auto f1 = LTL(root->left);
      auto f2 = LTL(root->right);
      // f1' R f2, for f1' in WR(f1)
      auto f1_dots = WR(f1, lasso);
      for(auto& f1_dot: f1_dots) {
        ret.emplace(f1_dot.release(f2));
      }
      // f1 R f2' for f2' in WR(f2)
      auto f2_dots = WR(f2, lasso);
      for(auto& f2_dot: f2_dots) {
        ret.emplace(f1.release(f2_dot));
      }
      // f1 | f2
      ret.emplace(f1.oor(f2));
      // f2
      ret.emplace(f2);
    }

    // 2. 选择未在f中出现的lasso的状态，析取term
    if(!lasso.always_false && formula.is_boolean_formula()) {
      auto all_literals = get_all_literals(formula);
      bool should_insert = false;

      for(auto& literal: all_literals) {
        for(auto& term: lasso.terms) {
          if(literal.first != term.first && literal.second == term.second) {
            should_insert = true;
            break;
          }
        }
        if(!should_insert) {
          bool found = false;
          for(auto& term: lasso.terms) {
            if(literal.second == term.second) {
              found = true;
              break;
            }
          }
          if(!found) {
            should_insert = true;
          }
        }
      }
      if(should_insert) {
        ret.emplace(formula.oor(lasso.to));
      }
    }
    return ret;
  }
}
