#pragma once

#include <string>

namespace autobc {
  class Operator {};

  class Op1: public Operator {};

  class Op2: public Operator {};

  class EmptyOp: public Op1, public Op2 {};

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
}
