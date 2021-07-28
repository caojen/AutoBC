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
    this->preOps.clear();
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
}
