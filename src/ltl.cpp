#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <set>

#include "ltl.hpp"
#include "error.hpp"
#include "dict.hpp"

namespace ltl {
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
      throw not_a_ltl();
    }
    LTLGenerator ret = *this;
    ret.preOps.push_front(op::next);

    return ret;
  }

  LTLGenerator LTLGenerator::finally() const {
    if(this->empty()) {
      throw not_a_ltl();
    }

    LTLGenerator ret = *this;
    ret.preOps.push_front(op::finally);

    return ret;
  }

  LTLGenerator LTLGenerator::global() const {
    if(this->empty()) {
      throw not_a_ltl();
    }

    LTLGenerator ret = *this;
    ret.preOps.push_front(op::global);

    return ret;
  }

  LTLGenerator LTLGenerator::nnot() const {
    if(this->empty()) {
      throw not_a_ltl();
    }

    LTLGenerator ret = *this;
    ret.preOps.push_front(op::nnot);

    return ret;
  }

  LTLGenerator LTLGenerator::until(const LTLGenerator& ltl) const {
    if(this->empty()) {
      throw not_a_ltl();
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
      throw not_a_ltl();
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
      throw not_a_ltl();
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
      throw not_a_ltl();
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
    return this->li == "" && this->left &&
      this->mid && this->right;
  }

  bool LTLGenerator::singal() const {
    return !this->empty() && this->li != "";
  }

  LTLGenerator LTLGenerator::Gen(const LTLGenerator& ltl) {
    return ltl;
  }

  LTL LTLGenerator::parse(const std::string& s) {
    return LTL::parse(s);
  }

  bool LTL::LTLNode::is_literal() const {
    return this->op == op::emptyOp;
  }

  bool LTL::LTLNode::is_op1() const {
    return this->op == op::finally ||
      this->op == op::global ||
      this->op == op::next ||
      this->op == op::nnot;
  }

  bool LTL::LTLNode::is_op2() const {
    return this->op == op::aand ||
      this->op == op::oor ||
      this->op == op::release ||
      this->op == op::until;
  }

  std::ostream& operator<<(std::ostream& o, const LTL::LTLNode& ltlNode) {
    o << ltlNode.serialize();
    return o;
  }

  std::string LTL::LTLNode::serialize() const {
    std::ostringstream ostr("");

    if(this->is_literal()) {
      ostr << this->literal->serialize();
    } else if(this->is_op1()) {
      ostr << this->op->str();
      ostr << "(";
      ostr << this->right->serialize();
      ostr << ")";
    } else if(this->is_op2()) {
      ostr << "(";
      ostr << this->left->serialize();
      ostr << ")";
      ostr << this->op->str();
      ostr << "(";
      ostr << this->right->serialize();
      ostr << ")";
    } else {
      throw not_a_ltl();
    }
  }
}
