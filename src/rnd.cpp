#include <queue>

#include "rnd.hpp"
#include "sat.hpp"

namespace rnd {
  Range::Range(unsigned int right) {
    this->left = 0;
    this->right = right;
  }

  Range::Range(unsigned int left, unsigned int right) {
    this->left = left;
    this->right = right;
  }

  Rander::Rander(const RanderTime& rt) {
    this->rt = rt;
    for(unsigned i = 0; i < 3; i++) {
      char ch = 'f' + i;
      this->charset.emplace_back(1, ch);
    }
    this->charset.emplace_back("1");
    this->charset.emplace_back("0");
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

  RanderCNF::RanderCNF(const RanderTime &rt): Rander(rt) {}

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

//    std::cout << "Initialize LTL: " << ltl.serialize() << std::endl;
    unsigned step = 0;
    while(ret.size() < k && !leaves.empty() && ++step) {
//      std::cout << step<< ": Remaining leaves size = " << leaves.size() << std::endl;
      std::shared_ptr<LTL::LTLNode> subClause = leaves.front(); leaves.pop();
//      std::cout << step << ": Dealing with sub clause: " << subClause->serialize() << std::endl;

      // 判断是要替换op还是操作数
      if(next_li()) {
//         std::cout << step << ": Replace Literal" << std::endl;
         std::string li;
         do {
           li = randLiteral();
         } while(dict.get(li) == subClause->literal);
         subClause->literal = dict.get(li);
         leaves.push(subClause);
      } else {
//        std::cout << step << ": Replace Operator" << std::endl;
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
//      std::cout << step << ": Sub Clause Changed to " << subClause->serialize() << std::endl;
//      std::cout << step << ": LTL Changed to " << ltl.serialize() << " (depth: " << ltl.depth() << ")" << std::endl;

      // 判断新的LTL是不是SAT
      if(!satSolver->checkSAT(ltl)) {
        // UnSAT
        std::string unSAT = ltl.serialize();
        std::cout << step << ": Found UnSAT: " << unSAT << " (depth:" << ltl.depth() << ")" << std::endl;
        std::string SAT;
        bool found = false;
        unsigned try_time = 100;        // 最多尝试100次
        if(subClause->is_literal()) {   // 如果子句只是一个字符，那么替换这个字符，尝试获得一个SAT
          do {
            std::shared_ptr<Literal> li = subClause->literal;
            std::string nl;
            do {
              nl = randLiteral();
            } while(nl == li->serialize());
            subClause->literal = dict.get(nl);
            if(satSolver->checkSAT((ltl))) {
              found = true;
            }
          } while(!found && try_time--);
        } else if(subClause->is_op1() || subClause->is_op2()) { // 子句是运算符，那么重构这个运算符
          do {
            // 随机一个运算符，可以与当前重复
            subClause->op = randOperator();
            if(dynamic_cast<Op1*>(subClause->op.get()) != nullptr) {
              subClause->left = nullptr;
              std::string s = randLiteral();
              subClause->right = std::make_shared<LTL::LTLNode>(s);
            } else if(dynamic_cast<Op2*>(subClause->op.get()) != nullptr) {
              std::string s1 = randLiteral();
              std::string s2 = randLiteral();
              subClause->left = std::make_shared<LTL::LTLNode>(s1);
              subClause->right = std::make_shared<LTL::LTLNode>(s2);
            } else {
              throw unreachable();
            }
            if(satSolver->checkSAT(ltl)) {
              found = true;
            }
          } while(!found && try_time--);
        } else {
          throw unreachable();
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
            } else {
              std::cout << "Existed. Skipped..." << std::endl;
            }
          }
          leaves.push(subClause);
        } else {      // 没有找到任何替换的可能性
          // throw unreachable();    // TODO: 没有任何替换的可能性时，寻找一种方法来继续算法
          // 目前，只退出即可
          break;
        }
      } else {
//        std::cout << step << ": LTL is SAT" << std::endl;
      }
      if(step % 100 == 0) {
        std::cout << step << ": Found Pairs = " << ret.size() << std::endl;
      }

      // 检查是否超时
      auto end = std::chrono::system_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
      if(duration.count() > this->rt.count()) {
        std::cout << "Expire at Step: " << step << std::endl;
        break;
      }
    }

    std::cout << "RanderCNF Done. Return Size = " << ret.size() << std::endl;
    return ret;
  }
}
