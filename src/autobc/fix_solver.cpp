#include <chrono>
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
  FixSolver::FixSolver(const std::set<ltl::LTL>& domains, const ltl::LTL& goal, const Lasso& bc, const std::set<LTL>& old_goals, bool goal_is_from_domain) {
    this->domains = domains;
    this->goal = goal;
    this->bc = bc;
    this->level = 0;
    this->old_goals = old_goals;
    this->goal_is_from_domain = goal_is_from_domain;
  }

  const std::set<ltl::LTL>& FixSolver::fix_with_limit(unsigned limit) {
    auto start = std::chrono::system_clock::now();
    this->fix_result.clear();
    
    std::queue<ltl::LTL> cs;
    std::queue<ltl::LTL> cw;

    std::set<ltl::LTL> cs_used;
    std::set<ltl::LTL> cw_used;

    unsigned check_time = 0;

    cs.push(this->goal);
    cw.push(this->goal);

    auto cur = std::chrono::system_clock::now();

    while((!cs.empty() || !cw.empty()) && check_time < limit && cur - start < std::chrono::minutes(30)) {
      if(!cs.empty()) {
        auto c = cs.front(); cs.pop();
        auto Thi = FixSolver::SR(c, this->bc);
        for(auto& thi: Thi) {
          if(cs_used.find(thi) != cs_used.end()) {
            continue;
          } else {
            cs_used.insert(thi);
          }
          check_time++;
          if(FixSolver::SR_repair_success(thi, this->domains, this->old_goals, this->bc.ltl, this->goal_is_from_domain)) {
            std::cout << "+ Get Fix Result From SR: " << thi << std::endl;
            this->fix_result.insert(thi);
          } else {
            std::cout << "- WR found, push into cw queue: " << thi << std::endl;
            cs.push(thi);
          }
        }
      }

      if(!cw.empty()) {
        auto c = cw.front(); cw.pop();
        auto Thi = FixSolver::WR(c, this->bc);
        for(auto& thi: Thi) {
          if(cw_used.find(thi) != cw_used.end()) {
            continue;
          } else {
            cw_used.insert(thi);
          }
          check_time++;
          if(FixSolver::WR_repair_success(thi, this->domains, this->old_goals, this->bc.ltl)) {
            std::cout << "+ Get Fix Result From WR: " << thi << std::endl;
            this->fix_result.insert(thi);
          } else {
            std::cout << "- WR found, push into cw queue: " << thi << std::endl;
            cw.push(thi);
          }
        } 
      }
      cur = std::chrono::system_clock::now();
    }
    
    return this->fix_result;
  }

  std::set<LTL> FixSolver::SR(const LTL& formula, Lasso& lasso) {
    std::set<LTL> ret;

    auto& root = formula.root;

    // SR(f) = 

    // 1: G(f)
    // ret.emplace(formula.global());
    // 3. if f = f1 | f2
    if(root->op == op::oor) {
      // f1:
      // ret.emplace(root->left);
      // f2:
      // ret.emplace(root->right);
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
      // ret.emplace(f1);
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
      // ret.emplace(f2);
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
    if(formula.is_boolean_formula()) {
      auto const terms = lasso.fetch_terms(1);
      auto insert_func = [&](unsigned begin) {
        for(; begin < terms.size(); ++begin) {
          auto const & term = terms.at(begin);

          auto workspace = formula.aand(term);
          if(satSolver->checkSAT(workspace) == true) {
            ret.emplace(formula.aand(term.nnot()));
          }
        }
      };
      
      if(formula.root->op != op::aand) {
        insert_func(0);
      } else {
        // 检查right是否为一个! term
        auto right = formula.root->right;
        if(right->is_op1() && right->op == op::nnot) {
          // 是一个 ! term形式的， 检查这个是不是一个term
          auto item = LTL(right->right);
          bool found = false;
          for(unsigned i = 0; i < terms.size() && found == false; ++i) {
            if(item == terms[i]) {
              found = true;
              insert_func(i + 1);
            }
          }
          if(!found) {
            insert_func(0);
          }
        } else {
          insert_func(0);
        }
      }
    }
    return ret;
  }

  std::set<LTL> FixSolver::WR(const LTL& formula, Lasso& lasso) {
    std::set<LTL> ret;
    std::set<LTL> lasso_ret;

    auto& root = formula.root;

    // WR(f) = 
    // 1. F f
    // ret.emplace(formula.finally());

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
      // ret.emplace(f1);
      // f2
      // ret.emplace(f2);
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
      // ret.emplace(f1);
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
      // ret.emplace(f2);
    }

    // 2. 选择未在f中出现的lasso的状态，析取term
    if(formula.is_boolean_formula()) {
      auto terms = lasso.fetch_terms(1);
      auto insert_func = [&](unsigned begin) {
        for(; begin < terms.size(); ++begin) {
          auto const & term = terms.at(begin);
          ret.emplace(formula.oor(term));
        }
      };

      if(formula.root->op != op::oor) {
        insert_func(0);
      } else {
        // 检查right是否为一个 term
        auto right = LTL(formula.root->right);
        bool found = false;
        for(unsigned i = 0; i < terms.size() && found == false; ++i) {
          if(right == terms[i]) {
            found = true;
            insert_func(i + 1);
          }
        }
        if(!found) {
          insert_func(0);
        }
      }
    }
    return ret;
  }

  bool FixSolver::SR_repair_success(const ltl::LTL& formula, const std::set<ltl::LTL>& domains, const std::set<ltl::LTL>& goals, const ltl::LTL& bc, bool goal_is_from_domain) {
    auto combine = bc;
    for(auto& domain: domains) {
      combine = combine.aand(domain);
    }
    if(goal_is_from_domain) {
      combine.aand(formula);
    }

    // goals to vec_goals
    std::vector<ltl::LTL> vec_goals;
    for(auto goal: goals) {
      vec_goals.push_back(goal);
    }

    bool get_unsat = false;

    if(vec_goals.size() > 1) {

      C c(vec_goals.size() - 1, vec_goals.size());
      auto cns = c.next();

      while(!cns.empty() && !get_unsat) {
        ltl::LTL tmp = combine;
        for(auto& cn: cns) {
          tmp = tmp.aand(vec_goals.at(cn));
        }
        if(!goal_is_from_domain) {
          tmp = tmp.aand(formula);
        }
        if(satSolver->checkSAT(tmp) == false) {
          get_unsat = true;
        }
        cns = c.next();
      }

    } else if(vec_goals.size() == 1) {
      // vec_goals.size() == 1
      combine = combine.aand(vec_goals[0]);
      if(!goal_is_from_domain) {
        combine = combine.aand(formula);
      }
      if(satSolver->checkSAT(combine) == false) {
        get_unsat = true;
      }
    } else {
      // vec_goals.size() == 0
      if(!goal_is_from_domain) {
        combine = combine.aand(formula);
      }
      if(satSolver->checkSAT(combine) == false) {
        get_unsat = true;
      }
    }
    
    if(get_unsat) {
      return true;
    } else {
      return false;
    }
  }

  bool FixSolver::WR_repair_success(const ltl::LTL& formula, const std::set<ltl::LTL>& domains, const std::set<ltl::LTL>& goals, const ltl::LTL& bc) {
    auto combine = formula.aand(bc);
    for(auto& domain: domains) {
      combine = combine.aand(domain);
    }
    for(auto& goal: goals) {
      combine = combine.aand(goal);
    }

    if(satSolver->checkSAT(combine) == true) {
      return true;
    } else {
      return false;
    }
  }
}
