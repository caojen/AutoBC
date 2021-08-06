#include <queue>
#include <algorithm>
#include <random>

#include "rander/rnd.hpp"
#include "ltl/sat.hpp"

namespace rnd {
  Range::Range(unsigned int right) {
    this->left = 0;
    this->right = right;
  }

  Range::Range(unsigned int left, unsigned int right) {
    this->left = left;
    this->right = right;
  }

  Rander::Rander(const RanderTime& rt, int vocabLen) {
    this->rt = rt;
    vocabLen = std::min(vocabLen, 25);
    for(int i = 0; i < vocabLen; i++) {
      char ch = 'b' + i;
      this->charset.emplace_back(1, ch);
    }
//    this->charset.emplace_back("1");
//    this->charset.emplace_back("0");

    std::cout << "Rander Charset:" << std::endl;
    for(auto i: this->charset) {
      std::cout << i << ", ";
    }
    std::cout << std::endl;
  }

  void Rander::set_limit(const RanderTime &rt) {
    this->rt = rt;
  }

  std::set<LTLPair> Rander::clear() {
    std::set<LTLPair> ret = std::move(this->ltls);
    this->ltls.clear();
    return ret;
  }

  Rander::~Rander() {}

  RanderCNF::RanderCNF(const RanderTime &rt, int vocabLen): Rander(rt, vocabLen) {}

  std::set<LTLPair> RanderCNF::operator()(unsigned k, const Range& r) {
    std::cout << "RanderCNF started. Limited " << this->rt.count() << "s to generate " << k << " LTL pairs ";
    std::cout << "in depth [" << r.left << ", " << r.right << ")." << std::endl;

    auto start = std::chrono::system_clock::now();

    std::set<LTLPair> ret;

    // < cmp  表明需要生成文字， > cmp 表明需要生成符号
    unsigned cmp = 50;
    unsigned lim = 100;

    auto inc = [&]() { cmp = std::min((cmp + std::rand() % 30), lim); };
    auto dec = [&]() { cmp = std::min((cmp - std::rand() % 30), lim); };

    auto next_li = [&]() -> bool {
      unsigned t = std::rand() % lim;
      if(t >= cmp) {
        inc();
        return false;
      } else {
        dec();
        return true;
      }
    };

    using namespace ltl;

    std::vector<std::shared_ptr<Operator>> ops;
    ops.push_back(op::aand);
    ops.push_back(op::finally);
    ops.push_back(op::global);
    ops.push_back(op::next);
    ops.push_back(op::nnot);
    ops.push_back(op::oor);
//    ops.push_back(op::release);
    ops.push_back(op::until);

    auto randLiteral = [&]() -> std::string { return this->charset.at(std::rand() % this->charset.size()); };
    auto randOperator = [&]() -> std::shared_ptr<ltl::Operator> { return ops.at(std::rand() % ops.size()); };

    std::queue<std::shared_ptr<LTL::LTLNode>> leaves;
    LTL ltl;

    while(true) {
      // 初始化第一个节点，while(1)确保初始化的时候总是sat
      if(next_li()) {     // 如果第一个节点需要生成文字
        std::string li = randLiteral();
        ltl.root = std::make_shared<LTL::LTLNode>(li);
        leaves.push(ltl.root);
      } else {            // 否则，第一个节点需要生成一个符号
        auto op = randOperator();
        // 判断这个op是一元还是二元，然后根据这个op来随机生成一个或两个操作数
        if(dynamic_cast<Op1*>(op.get()) != nullptr) {
          auto li = randLiteral();
          ltl.root = std::make_shared<LTL::LTLNode>();
          ltl.root->op = op;
          ltl.root->right = std::make_shared<LTL::LTLNode>(li);
          leaves.push(ltl.root->right);
        } else if(dynamic_cast<Op2*>(op.get()) != nullptr) {
          auto li1 = randLiteral();
          auto li2 = randLiteral();
          ltl.root = std::make_shared<LTL::LTLNode>();
          ltl.root->op = op;
          ltl.root->left = std::make_shared<LTL::LTLNode>(li1);
          ltl.root->right = std::make_shared<LTL::LTLNode>(li2);
          leaves.push(ltl.root->left);
          leaves.push(ltl.root->right);
        } else {
          throw unreachable();
        }
      }
      // 判断得到的LTL是不是sat的，如果不是，重做
      if(satSolver->checkSAT(ltl)) {
        break;
      } else {
        ltl.root = nullptr;
      }
    }

    unsigned step = 0;
    while(ret.size() < k && !leaves.empty() && ++step) {
      std::shared_ptr<LTL::LTLNode> subClause = leaves.front(); leaves.pop();

      // 判断是要替换op还是操作数
      if(next_li()) {
        // 随机找一个文字来替换
         std::string li;
         do {
           li = randLiteral();
         } while(dict.get(li) == subClause->literal);
         subClause->literal = dict.get(li);
         leaves.push(subClause);
      } else {
        subClause->literal = nullptr;
        // 随机一个op并替换到当前的子句中
        std::shared_ptr<Operator> op = randOperator();
        if(dynamic_cast<Op1*>(op.get()) != nullptr) {
          std::string li = randLiteral();
          subClause->op = op;
          subClause->right = std::make_shared<LTL::LTLNode>(li);
          leaves.push(subClause->right);
        } else if(dynamic_cast<Op2*>(op.get()) != nullptr) {
          std::string li1 = randLiteral();
          std::string li2 = randLiteral();
          subClause->op = op;
          subClause->left = std::make_shared<LTL::LTLNode>(li1);
          subClause->right = std::make_shared<LTL::LTLNode>(li2);
          leaves.push(subClause->left);
          leaves.push(subClause->right);
        } else {
          throw unreachable();
        }
      }

      // 判断新的LTL是不是SAT
      if(!satSolver->checkSAT(ltl)) {
        // UnSAT
        std::string unSAT = ltl.serialize();
        std::cout << step << ": Found UnSAT: " << unSAT << " (depth:" << ltl.depth() << ")" << std::endl;
        std::string SAT;
        bool found = false;

        // 找到这个LTL的层序遍历
        auto level_order = ltl.get_level_order();
        std::random_shuffle(level_order.begin(), level_order.end());
        for(auto &node: level_order) {
          if(node->is_literal()) {
            auto old_li = node->literal;
            std::vector<std::string> literals;
            for(auto& ch: this->charset) {
              if(dict.get(ch) != node->literal) {
                literals.push_back(ch);
              }
            }
            std::random_shuffle(literals.begin(), literals.end());
            for(auto& literal: literals) {
              node->literal = dict.get(literal);
              if(satSolver->checkSAT(ltl)) {
                found = true;
                break;
              } else {
                node->literal = old_li;
              }
            }
          } else if(node->is_op1()) {

            auto old_op = node->op;
            std::vector<std::shared_ptr<ltl::Operator>> ops;
            if(node->op != op::nnot) {
              ops.push_back(op::nnot);
            } else if(node->op != op::next) {
              ops.push_back(op::next);
            } else if(node->op != op::global) {
              ops.push_back(op::global);
            } else if(node->op != op::finally) {
              ops.push_back(op::global);
            }
            std::random_shuffle(ops.begin(), ops.end());

            for(auto& op: ops) {
              node->op = op;
              if(satSolver->checkSAT(ltl)) {
                found = true;
                break;
              } else {
                node->op = old_op;
              }
            }
          } else if(node->is_op2()) {
            auto old_op = node->op;
            std::vector<std::shared_ptr<ltl::Operator>> ops;
            if(node->op != op::until) {
              ops.push_back(op::until);
            } else if(node->op != op::oor) {
              ops.push_back(op::oor);
            } else if(node->op != op::aand) {
              ops.push_back(op::aand);
            }
            std::random_shuffle(ops.begin(), ops.end());
            for(auto& op: ops) {
              node->op = op;
              if(satSolver->checkSAT(ltl)) {
                found = true;
                break;
              } else {
                node->op = old_op;
              }
            }

          } else {
            throw unreachable();
          }
          if(found) {
            break;
          }
        }

        if(found) {   // 成功找到了SAT，推到结果中，然后将SAT应用到LTL中继续寻找
          SAT = ltl.serialize();
          std::cout << step << ": Found SAT: " << SAT << std::endl;
          auto depth = ltl.depth();
          if(depth >= r.left && depth < r.right) {
            auto before = this->ltls.size();
            this->ltls.insert(std::pair<std::string, std::string>(SAT, unSAT));
            auto after = this->ltls.size();
            if(before != after) {
              ret.insert(std::pair<std::string, std::string>(SAT, unSAT));
              if(SAT == unSAT) {
                std::cout << "找到了两个相同的LTL，但是标记不同" << std::endl;
                throw std::runtime_error("Fatal");
              }
            } else {
              std::cout << "Existed. Skipped..." << std::endl;
            }
          }
          leaves.push(subClause);
        } else {
          std::cout << "Break because cannot found a sat" << std::endl;
          std::cout << "在第一层替换中没有找到可替换的内容" << std::endl;
          abort();
        }
      }
      else {
//        std::cout << step << " SAT " << ltl << std::endl;
      }
      // 检查是否超时
      auto end = std::chrono::system_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
      if(duration.count() > this->rt.count()) {
        std::cout << "Timeout at Step: " << step << std::endl;
        break;
      }
      if(ltl.depth() > r.right) {
        std::cout << "Depth Limit at Step: " << step << std::endl;
        break;
      }
    }

    std::cout << "RanderCNF Done. Return Size = " << ret.size() << std::endl;
    return ret;
  }
}
