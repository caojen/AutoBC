#include <random>
#include <queue>
#include <set>
#include <chrono>

#include "random_solver.hpp"
#include "sat.hpp"

using namespace autobc;

static inline unsigned random_range(unsigned begin, unsigned end) {
  std::random_device r;
  std::default_random_engine e(r());
  std::uniform_int_distribution<unsigned> u(begin, end - 1);
  return u(e);
}

static inline bool random_boolean() {
  std::random_device r;
  std::default_random_engine e(r());
  std::bernoulli_distribution u(0.5);
  return u(e);
}

auto t = random_boolean();

RandomSolver::RandomSolver(const std::set<ltl::LTL>& domains, const ltl::LTL& goal, const Lasso& bc, const std::set<ltl::LTL>& old_goals) {
  this->domains = domains;
  this->goal = goal;
  this->bc = bc;
  this->goals = old_goals;
}

const std::set<ltl::LTL>& RandomSolver::fix_with_limit(unsigned limit) {
  this->fix_results.clear();

  std::set<ltl::LTL>& result = this->fix_results;
  std::set<ltl::LTL> used;
  std::queue<ltl::LTL> queue;
  auto rand_time = 100;
  while(rand_time--) {
    queue = {};
    used = {};
    queue.push(this->goal);
    auto prev = std::chrono::system_clock::now();
    auto curr = std::chrono::system_clock::now();

    while(result.size() < limit && !queue.empty() && curr - prev < std::chrono::seconds(60)) {
      auto f = queue.front(); queue.pop();
      if(used.find(f) != used.end()) {
        continue;
      } else {
        used.insert(f);
      }

      auto tmp = RS(f);
      for(auto &t: tmp) {
        if(this->repair_success(t)) {
          std::cout << "+ RS result found: " << t << std::endl;
          result.insert(t);
        } else {
          std::cout << "- RS queue pushed: " << t << std::endl;
          queue.push(t);
        }
      }
    }
  }

  return this->fix_results;
}

std::set<ltl::LTL> RandomSolver::RS(const ltl::LTL& formula) {
  std::set<ltl::LTL> ret;
    std::array<std::shared_ptr<ltl::Operator>, 9> ops = {
      ltl::op::finally,
      ltl::op::next,
      ltl::op::global,
      ltl::op::nnot,
      ltl::op::until,
      ltl::op::release,
      ltl::op::aand,
      ltl::op::oor,
      ltl::op::emptyOp
    };

    auto target_op = ops[random_range(0, ops.size())];
    auto is_op1 = dynamic_cast<ltl::Op1*>(target_op.get()) != nullptr;
    auto is_op2 = dynamic_cast<ltl::Op2*>(target_op.get()) != nullptr;

    ltl::LTL f = formula;
    if(f.root->is_op1()) {
      if(is_op1) {
        ret.insert(randOp1(f.root->right, f.root->op));
        auto tmp = RS(f.root->right);
        for(auto& t: tmp) {
          ret.insert(randOp1(t, ltl::op::emptyOp));
        }
      } else if(is_op2) {
        ltl::LTL left = ltl::LTL(*ltl::dict.random_get());
        ret.insert(randOp2(left, ltl::op::emptyOp, f.root->right));
        auto tmp = RS(f.root->right);
        for(auto& t: tmp) {
          ret.insert(randOp2(left, ltl::op::emptyOp, t));
        }
        tmp = RS(left);
        for(auto& t: tmp) {
          ret.insert(randOp2(t, ltl::op::emptyOp, f.root->right));
        }
      } else {
        f = ltl::LTL(*ltl::dict.random_get());
        ret.insert(f);
      }
    } else if(f.root->is_op2()) {
      if(is_op1) {
        ret.insert(randOp1(f.root->right, ltl::op::emptyOp));
        f.root->left = nullptr;
        auto tmp = RS(f.root->right);
        for(auto& t: tmp) {
          ret.insert(randOp1(t, ltl::op::emptyOp));
        }
      } else if(is_op2) {
        ret.insert(randOp2(f.root->left, ltl::op::emptyOp, f.root->right));
        auto tmp = RS(f.root->left);
        for(auto& t: tmp) {
          ret.insert(randOp2(t, ltl::op::emptyOp, f.root->right));
        }
        tmp = RS(f.root->right);
        for(auto& t: tmp) {
          ret.insert(randOp2(f.root->left, ltl::op::emptyOp, t));
        }
      } else {
        f = ltl::LTL(*ltl::dict.random_get());
        ret.insert(f);
      }
    } else if(f.root->is_literal()) {
      if(is_op1) {
        if(target_op == ltl::op::finally) {
          ret.insert(f.finally());
        } else if(target_op == ltl::op::global) {
          ret.insert(f.global());
        } else if(target_op == ltl::op::next) {
          ret.insert(f.next());
        } else if(target_op == ltl::op::nnot) {
          ret.insert(f.nnot());
        }
      } else if(is_op2) {
        if(target_op == ltl::op::aand) {
          ret.insert(f.aand(ltl::LTL(*ltl::dict.random_get())));
        } else if(target_op == ltl::op::oor) {
          ret.insert(f.oor(ltl::LTL(*ltl::dict.random_get())));
        } else if(target_op == ltl::op::until) {
          ret.insert(f.until(ltl::LTL(*ltl::dict.random_get())));
        } else if(target_op == ltl::op::release) {
          ret.insert(f.release(ltl::LTL(*ltl::dict.random_get())));
        }
      } else {
        f = ltl::LTL(*ltl::dict.random_get());
        ret.insert(f);
      }
    }
  return ret;
}

bool RandomSolver::repair_success(const ltl::LTL& formula) {
  bool ret = false;
  {
    auto workspace = formula.aand(this->bc.ltl);
    for(auto& domain: this->domains) {
      workspace = workspace.aand(domain);
    }
    for(auto& goal: this->goals) {
      workspace = workspace.aand(goal);
    }
    ret = ltl::satSolver->checkSAT(workspace);
  }
  
  if(ret == false) {
    std::vector<ltl::LTL> vec_goals;
    for(auto goal: goals) {
      vec_goals.push_back(goal);
    }
    if(vec_goals.size() > 1) {
      C c(vec_goals.size() - 1, vec_goals.size());
      auto cns = c.next();
      while(ret == false && !cns.empty()) {
        ltl::LTL combine = this->bc.ltl.aand(formula);
        for(auto& domain: this->domains) {
          combine = combine.aand(domain);
        }
        for(auto& cn: cns) {
          combine = combine.aand(vec_goals.at(cn));
        }
        if(ltl::satSolver->checkSAT(combine) == false) {
          ret = true;
        }
        cns = c.next();
      }
    } else if(vec_goals.size() == 1) {
      ltl::LTL combine = this->bc.ltl.aand(vec_goals[0]).aand(formula);
      for(auto& domain: this->domains) {
        combine = combine.aand(domain);
      }
      if(ltl::satSolver->checkSAT(combine) == false) {
        ret = true;
      }
    } else {
      // vec_goals . size () == 0
      ltl::LTL combine = this->bc.ltl.aand(formula);
      for(auto& domain: this->domains) {
        combine = combine.aand(domain);
      }
      if(ltl::satSolver->checkSAT(combine) == false) {
        ret = true;
      }
    }
  }

  return ret;
}

ltl::LTL RandomSolver::randOp1(const ltl::LTL& f, std::shared_ptr<ltl::Operator> op) {
  std::array<std::shared_ptr<ltl::Operator>, 4> ops = {
    ltl::op::finally,
    ltl::op::next,
    ltl::op::global,
    ltl::op::nnot
  };

  std::vector<std::shared_ptr<ltl::Operator>> vec;
  for(auto& item: ops) {
    if(item != op) {
      vec.push_back(item);
    }
  }

  auto c = vec.at(random_range(0, vec.size()));
  if(c == ltl::op::finally) {
    return f.finally();
  } else if(c == ltl::op::next) {
    return f.next();
  } else if(c == ltl::op::global) {
    return f.global();
  } else if(c == ltl::op::nnot) {
    return f.nnot();
  } else {
    return f;
  }
}
ltl::LTL RandomSolver::randOp2(const ltl::LTL& f1, std::shared_ptr<ltl::Operator> op, const ltl::LTL& f2) {
  std::array<std::shared_ptr<ltl::Operator>, 4> ops = {
    ltl::op::until,
    ltl::op::release,
    ltl::op::aand,
    ltl::op::oor
  };

  std::vector<std::shared_ptr<ltl::Operator>> vec;
  for(auto& item: ops) {
    if(item != op) {
      vec.push_back(item);
    }
  }

  auto c = vec.at(random_range(0, vec.size()));
  if(c == ltl::op::until) {
    return f1.until(f2);
  } else if(c == ltl::op::release) {
    return f1.release(f2);
  } else if(c == ltl::op::aand) {
    return f1.aand(f2);
  } else if(c == ltl::op::oor) {
    return f1.oor(f2);
  } else {
    // unreachable!
    return f1;
  }
}
