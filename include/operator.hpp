#pragma once

#include <string>
#include <memory>

namespace autobc {
  class Operator {};

  class Op1: public Operator {};

  class Op2: public Operator {};

  class EmptyOp: public Op1 {};

  class LTLOp1: public Op1 {};

  class LTLOp2: public Op2 {};

  class ProOp1: public Op1 {};

  class ProOp2: public Op2 {};

  class Next: public LTLOp1 {};

  class Finally: public LTLOp1 {};

  class Global: public LTLOp1 {};

  class Until: public LTLOp2 {};

  class Release: public LTLOp2 {};

  class Not: public ProOp1 {};

  class And: public ProOp2 {};

  class Or: public ProOp2 {};

  namespace op {

    extern std::shared_ptr<EmptyOp> emptyOp;
    extern std::shared_ptr<Next> next;
    extern std::shared_ptr<Finally> finally;
    extern std::shared_ptr<Global> global;
    extern std::shared_ptr<Until> until;
    extern std::shared_ptr<Release> release;
    extern std::shared_ptr<Not> nnot;
    extern std::shared_ptr<And> aand;
    extern std::shared_ptr<Or> oor;

  }
}
