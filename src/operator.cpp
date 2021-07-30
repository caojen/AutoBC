#include "operator.hpp"

namespace ltl {
  namespace op {
    std::shared_ptr<EmptyOp> emptyOp = std::make_shared<EmptyOp>();
    std::shared_ptr<Next> next = std::make_shared<Next>();
    std::shared_ptr<Finally> finally = std::make_shared<Finally>();
    std::shared_ptr<Global> global = std::make_shared<Global>();
    std::shared_ptr<Until> until = std::make_shared<Until>();
    std::shared_ptr<Release> release = std::make_shared<Release>();
    std::shared_ptr<Not> nnot = std::make_shared<Not>();
    std::shared_ptr<And> aand = std::make_shared<And>();
    std::shared_ptr<Or> oor = std::make_shared<Or>();

    std::ostream& operator<<(std::ostream& o, const Operator& ope) {
      o << ope.str();
      return o;
    }
  }

  std::shared_ptr<Operator> Operator::gen(const std::string& s) {
    if(s == "X") {
      return op::next;
    } else if(s == "F") {
      return op::finally;
    } else if(s == "G") {
      return op::global;
    } else if(s == "U") {
      return op::until;
    } else if(s == "R") {
      return op::release;
    } else if(s == "!") {
      return op::nnot;
    } else if(s == "&") {
      return op::aand;
    } else if(s == "|") {
      return op::oor;
    } {
      return op::emptyOp;
    }
  }
}