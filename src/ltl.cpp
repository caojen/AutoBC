#include <sstream>
#include <iostream>

#include "ltl.hpp"
#include "error.hpp"

namespace autobc {
  LTL::LTL() {
    this->preOps.clear();
    this->li = "";
    this->left = nullptr;
    this->mid = nullptr;
    this->right = nullptr;
  }

  LTL::LTL(const Literal& li) {
    this->preOps.clear();
    this->li = li;
    this->left = nullptr;
    this->mid = nullptr;
    this->right = nullptr;
  }

  LTL::LTL(const LTL& pro) {
    this->preOps = pro.preOps;
    this->li = pro.li;
    this->left = pro.left;
    this->mid = pro.mid;
    this->right = pro.right;
  }

  LTL LTL::next() const {
    if(this->empty()) {
      throw new not_a_ltl();
    }
    LTL ret = *this;
    ret.preOps.push_front(op::next);

    return ret;
  }

  LTL LTL::finally() const {
    if(this->empty()) {
      throw new not_a_ltl();
    }

    LTL ret = *this;
    ret.preOps.push_front(op::finally);

    return ret;
  }

  LTL LTL::global() const {
    if(this->empty()) {
      throw new not_a_ltl();
    }

    LTL ret = *this;
    ret.preOps.push_front(op::global);

    return ret;
  }

  LTL LTL::nnot() const {
    if(this->empty()) {
      throw new not_a_ltl();
    }

    LTL ret = *this;
    ret.preOps.push_front(op::nnot);

    return ret;
  }

  LTL LTL::until(const LTL& ltl) const {
    if(this->empty()) {
      throw new not_a_ltl();
    }

    std::shared_ptr<LTL> left(new LTL(*this));
    std::shared_ptr<Op2> mid = op::until;
    std::shared_ptr<LTL> right(new LTL(ltl));

    LTL ret;
    ret.left = left;
    ret.mid = mid;
    ret.right = right;

    return ret;
  }

  LTL LTL::release(const LTL& ltl) const {
    if(this->empty()) {
      throw new not_a_ltl();
    }

    std::shared_ptr<LTL> left(new LTL(*this));
    std::shared_ptr<Op2> mid = op::release;
    std::shared_ptr<LTL> right(new LTL(ltl));

    LTL ret;
    ret.left = left;
    ret.mid = mid;
    ret.right = right;

    return ret;
  }

  LTL LTL::aand(const LTL& ltl) const {
    if(this->empty()) {
      throw new not_a_ltl();
    }

    std::shared_ptr<LTL> left(new LTL(*this));
    std::shared_ptr<Op2> mid = op::aand;
    std::shared_ptr<LTL> right(new LTL(ltl));

    LTL ret;
    ret.left = left;
    ret.mid = mid;
    ret.right = right;

    return ret;
  }

  LTL LTL::oor(const LTL& ltl) const {
    if(this->empty()) {
      throw new not_a_ltl();
    }

    std::shared_ptr<LTL> left(new LTL(*this));
    std::shared_ptr<Op2> mid = op::oor;
    std::shared_ptr<LTL> right(new LTL(ltl));

    LTL ret;
    ret.left = left;
    ret.mid = mid;
    ret.right = right;

    return ret;
  }

  LTL::~LTL() {}

  std::ostream& operator<<(std::ostream& o, const LTL& ltl) {
    o << ltl.serialize();

    return o;
  }

  std::string LTL::serialize() const {
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

  bool LTL::operator==(const LTL& other) const {
    return this->serialize() == other.serialize();
  }

  bool LTL::empty() const {
    return this->li == "" && this->left == nullptr &&
      this->mid == nullptr && this->right == nullptr;
  }

  bool LTL::singal() const {
    return !this->empty() && this->li != "";
  }

  LTL LTL::Gen(const LTL& ltl) {
    return ltl;
  }
}
