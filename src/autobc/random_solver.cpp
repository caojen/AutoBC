#include "random_solver.hpp"

using namespace autobc;

RandomSolver::RandomSolver(const std::set<ltl::LTL>& domains, const ltl::LTL& goal, const Lasso& bc, const std::set<ltl::LTL>& old_goals) {
  this->domains = domains;
  this->goal = goal;
  this->bc = bc;
  this->goals = old_goals;
}

const std::set<ltl::LTL>& RandomSolver::fix_with_limit(unsigned limit) {
  return this->fix_results;
}

std::set<ltl::LTL> RandomSolver::RS(const ltl::LTL& formula) {
  return {};
}

bool RandomSolver::repair_success(const ltl::LTL& formula) {
  return false;
}
