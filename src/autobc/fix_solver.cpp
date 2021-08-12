#include "fix_solver.hpp"

using namespace ltl;

static auto get_all_literals = [](const ltl::LTL& f) -> std::set<std::shared_ptr<ltl::Literal>> {
  std::set<std::shared_ptr<ltl::Literal>> ret;
  std::queue<std::shared_ptr<ltl::LTL::LTLNode>> queue;
  if(f.root.get() == nullptr) {
    return ret;
  } else {
    queue.push(f.root);
  }

  while(!queue.empty()) {
    auto node = queue.front(); queue.pop();
    if(node->is_literal()) {
      ret.insert(node->literal);
    } else if(node->is_op1()) {
      queue.push(node->right);
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
  FixSolver::FixSolver(std::set<ltl::LTL>& domains, ltl::LTL& goal, Lasso& bc) {
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
      std::cout << "Solving formula: " << formula << std::endl;
      auto wrs = WR(formula, bc);
      auto srs = SR(formula, bc);

      std::cout << "\tWR: " << wrs.size() << std::endl;
      for(auto& wr: wrs) {
        if(used.find(wr) == used.end()) {
          std::cout << "\t\t" << wr << std::endl;
          used.insert(wr);
          next.insert(wr);
        }
      }

      std::cout << "\tSR: " << srs.size() << std::endl;
      for(auto& sr: srs) {
        if(used.find(sr) == used.end()) {
          std::cout << "\t\t" << sr << std::endl;
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

    // 2. 选择在formula出现的可以蕴含lasso的状态去除
    if(!lasso.always_false && formula.is_boolean_formula()) {
      auto all_literals = get_all_literals(formula);
      bool exists = false;

      for(auto &literal: all_literals) {
        if(lasso.literals.find(literal) != lasso.literals.end()) {
          exists = true;
          break;
        }
      }
      if(!exists) {
        ret.emplace(formula.aand(lasso.to.nnot()));
      }
    }
    return ret;
  }

  std::set<LTL> FixSolver::WR(const LTL& formula, const Lasso& lasso) {
    return {};
  }
}
