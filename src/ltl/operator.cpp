#include "ltl/operator.hpp"

namespace ltl {
    bool format_double_and = false;
    bool format_double_or = false;
    bool format_symbol_F = false;
    bool format_symbol_G = false;

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
    std::shared_ptr<Imply> imply = std::make_shared<Imply>();
    std::shared_ptr<Equal> equal = std::make_shared<Equal>();

    std::ostream& operator<<(std::ostream& o, const Operator& ope) {
      o << ope.str();
      return o;
    }
  }

  std::shared_ptr<Operator> Operator::gen(const std::string& s) {
    if(s == "X") {
      return op::next;
    } else if(s == "F" || s == "<>") {
      return op::finally;
    } else if(s == "G" || s == "[]") {
      return op::global;
    } else if(s == "U") {
      return op::until;
    } else if(s == "R") {
      return op::release;
    } else if(s == "!") {
      return op::nnot;
    } else if(s == "&" || s == "&&") {
      return op::aand;
    } else if(s == "|" || s == "||") {
      return op::oor;
    } else if(s == "->") {
      return op::imply;
    } else if(s == "<->") {
      return op::equal;
    }

    return op::emptyOp;
  }
}