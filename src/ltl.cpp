#include <sstream>
#include <iostream>
#include <vector>
#include <string>

#include "ltl.hpp"
#include "error.hpp"

namespace autobc {
  LTLGenerator::LTLGenerator() {
    this->preOps.clear();
    this->li = "";
    this->left = nullptr;
    this->mid = nullptr;
    this->right = nullptr;
  }

  LTLGenerator::LTLGenerator(const Literal& li) {
    this->preOps.clear();
    this->li = li;
    this->left = nullptr;
    this->mid = nullptr;
    this->right = nullptr;
  }

  LTLGenerator::LTLGenerator(const LTLGenerator& pro) {
    this->preOps = pro.preOps;
    this->li = pro.li;
    this->left = pro.left;
    this->mid = pro.mid;
    this->right = pro.right;
  }

  LTLGenerator LTLGenerator::next() const {
    if(this->empty()) {
      throw new not_a_ltl();
    }
    LTLGenerator ret = *this;
    ret.preOps.push_front(op::next);

    return ret;
  }

  LTLGenerator LTLGenerator::finally() const {
    if(this->empty()) {
      throw new not_a_ltl();
    }

    LTLGenerator ret = *this;
    ret.preOps.push_front(op::finally);

    return ret;
  }

  LTLGenerator LTLGenerator::global() const {
    if(this->empty()) {
      throw new not_a_ltl();
    }

    LTLGenerator ret = *this;
    ret.preOps.push_front(op::global);

    return ret;
  }

  LTLGenerator LTLGenerator::nnot() const {
    if(this->empty()) {
      throw new not_a_ltl();
    }

    LTLGenerator ret = *this;
    ret.preOps.push_front(op::nnot);

    return ret;
  }

  LTLGenerator LTLGenerator::until(const LTLGenerator& ltl) const {
    if(this->empty()) {
      throw new not_a_ltl();
    }

    std::shared_ptr<LTLGenerator> left(new LTLGenerator(*this));
    std::shared_ptr<Op2> mid = op::until;
    std::shared_ptr<LTLGenerator> right(new LTLGenerator(ltl));

    LTLGenerator ret;
    ret.left = left;
    ret.mid = mid;
    ret.right = right;

    return ret;
  }

  LTLGenerator LTLGenerator::release(const LTLGenerator& ltl) const {
    if(this->empty()) {
      throw new not_a_ltl();
    }

    std::shared_ptr<LTLGenerator> left(new LTLGenerator(*this));
    std::shared_ptr<Op2> mid = op::release;
    std::shared_ptr<LTLGenerator> right(new LTLGenerator(ltl));

    LTLGenerator ret;
    ret.left = left;
    ret.mid = mid;
    ret.right = right;

    return ret;
  }

  LTLGenerator LTLGenerator::aand(const LTLGenerator& ltl) const {
    if(this->empty()) {
      throw new not_a_ltl();
    }

    std::shared_ptr<LTLGenerator> left(new LTLGenerator(*this));
    std::shared_ptr<Op2> mid = op::aand;
    std::shared_ptr<LTLGenerator> right(new LTLGenerator(ltl));

    LTLGenerator ret;
    ret.left = left;
    ret.mid = mid;
    ret.right = right;

    return ret;
  }

  LTLGenerator LTLGenerator::oor(const LTLGenerator& ltl) const {
    if(this->empty()) {
      throw new not_a_ltl();
    }

    std::shared_ptr<LTLGenerator> left(new LTLGenerator(*this));
    std::shared_ptr<Op2> mid = op::oor;
    std::shared_ptr<LTLGenerator> right(new LTLGenerator(ltl));

    LTLGenerator ret;
    ret.left = left;
    ret.mid = mid;
    ret.right = right;

    return ret;
  }

  LTLGenerator::~LTLGenerator() {}

  std::ostream& operator<<(std::ostream& o, const LTLGenerator& ltl) {
    o << ltl.serialize();

    return o;
  }

  std::string LTLGenerator::serialize() const {
    std::ostringstream o;

    // 输出所有前缀运算符
    for(auto& op: this->preOps) {
      o << op->str();
    }

    o << "(";

    // 判断这是不是一个文字，如果是，那么直接输出这个文字
    if(this->singal()) {
      o << this->li;
    } else {
      // 递归地输出第一部分:
      o << this->left->serialize();
      // 如果存在第二部分:
      if(this->mid != nullptr) {
        // 输出中间的运算符
        o << this->mid->str();
        if(this->right != nullptr) {
          // 递归地输出第二部分
          o << this->right->serialize();
        }
      }
    }
    o << ")";
    std::string ret = o.str();
    // 清除中的所有空格
    auto iter = ret.begin();
    while(iter != ret.end()) {
      if(*iter == ' ') {
        iter = ret.erase(iter);
      } else {
        ++iter;
      }
    }

    return ret;
  }

  bool LTLGenerator::operator==(const LTLGenerator& other) const {
    return this->serialize() == other.serialize();
  }

  bool LTLGenerator::empty() const {
    return this->li == "" && this->left == nullptr &&
      this->mid == nullptr && this->right == nullptr;
  }

  bool LTLGenerator::singal() const {
    return !this->empty() && this->li != "";
  }

  LTLGenerator LTLGenerator::Gen(const LTLGenerator& ltl) {
    return ltl;
  }

  LTL LTLGenerator::parse(const std::string& s) {
    return LTL::Gen(s);
  }

  LTL LTL::Gen(const std::string& s) {
    // remove all ' '

    std::string x = s;
    auto iter = x.begin();
    while(iter != x.end()) {
      if(*iter == ' ') {
        iter = x.erase(iter);
      } else {
        ++iter;
      }
    }

    // split x into splits
    std::cout << x << std::endl;

    std::vector<std::string> splits;

    auto is_special = [](const char ch) -> bool {
      return ch == '(' || ch == ')' || ch == '&' || ch == '|' || ch == '!';
    };

    auto might_special = [](const char ch) -> bool {
      return ch == 'X' || ch == 'U' || ch == 'R' || ch == 'F' || ch == 'G';
    };

    iter = x.begin();
    while(iter != x.end()) {
      if(is_special(*iter)) {
        splits.push_back(std::string(1, *iter));
        ++iter;
        continue;
      } else if(might_special(*iter) && (std::next(iter) == x.end() || is_special(*std::next(iter)))) {
        splits.push_back(std::string(1, *iter));
        ++iter;
        continue;
      } else {
        auto n = std::next(iter);
        while(n != x.end() && !is_special(*n)) {
          ++n;
        }

        splits.push_back(x.substr(iter - x.begin(), n - iter));
        iter = n;
      }
    }

    for(auto a: splits) {
      std::cout << a << ", ";
    }
    std::cout << std::endl;

    std::stack<int> stack;
    std::map<unsigned, unsigned> map;

    for(unsigned i = 0; i < splits.size(); i++) {
      if(splits[i] == "(") {
        stack.push(i);
      } else if(splits[i] == ")") {
        if(stack.empty()) {
          throw new not_a_ltl();
        } else {
          unsigned matched = stack.top(); stack.pop();
          map[i] = matched;
          map[matched] = i;
        }
      }
    }

    if(!stack.empty()) {
      throw new not_a_ltl();
    }

    return LTL::GenPart(splits, map, 0, splits.size());
  }

  LTL LTL::GenPart(const std::vector<std::string>& s, const std::map<unsigned, unsigned>& map, unsigned begin, unsigned end) {
    return LTL();
  }
}
