#pragma once

#include <string>
#include <memory>
#include <iostream>

namespace autobc {
  class Operator {
    public:
      friend std::ostream& operator<<(std::ostream& o, const Operator& ope);
      virtual std::string str() const = 0;
  };

  class Op1: public Operator {
    public:
      std::string str() const { return "Op1"; }
  };

  class Op2: public Operator {
    public:
      std::string str() const { return "Op2"; }
  };

  class EmptyOp: public Op1 {
    public:
      std::string str() const { return "EmptyOp"; }
  };

  class LTLOp1: public Op1 {
    public:
      std::string str() const { return "LTLOp1"; }
  };

  class LTLOp2: public Op2 {
    public:
      std::string str() const { return "LTLOp2"; }
  };

  class ProOp1: public Op1 {
    public:
      std::string str() const { return "ProOp1"; }
  };

  class ProOp2: public Op2 {
    public:
      std::string str() const { return "ProOp2"; }
  };

  class Next: public LTLOp1 {
    public:
      std::string str() const { return "X"; }
  };

  class Finally: public LTLOp1 {
    public:
      std::string str() const { return "F"; }
  };

  class Global: public LTLOp1 {
    public:
      std::string str() const { return "G"; }
  };

  class Until: public LTLOp2 {
    public:
      std::string str() const { return "U"; }
  };

  class Release: public LTLOp2 {
    public:
      std::string str() const { return "R"; }
  };

  class Not: public ProOp1 {
    public:
      std::string str() const { return "!"; }
  };

  class And: public ProOp2 {
    public:
      std::string str() const { return "&"; }
  };

  class Or: public ProOp2 {
    public:
      std::string str() const { return "|"; }
  };

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
