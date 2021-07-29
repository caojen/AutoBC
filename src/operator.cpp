#include "operator.hpp"

namespace autobc {
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
}