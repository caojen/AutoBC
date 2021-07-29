#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <sstream>

namespace ltl {
  class Operator {
    public:
      friend std::ostream& operator<<(std::ostream& o, const Operator& ope);
      virtual std::string str() const = 0;
      virtual ~Operator() = default;
      virtual unsigned weight() const = 0;
      static std::shared_ptr<Operator> gen(const std::string& s);
  };

  class Op1: public Operator {
    public:
      std::string str() const { return "Op1"; }
      unsigned weight() const { return 0; }
  };

  class Op2: public Operator {
    public:
      std::string str() const { return "Op2"; }
      unsigned weight() const { return 0; }
  };

  class EmptyOp: public Op1 {
    public:
      std::string str() const { return "EmptyOp"; }
      unsigned weight() const { return 0; }
  };

  class LTLOp1: public Op1 {
    public:
      std::string str() const { return "LTLOp1"; }
      unsigned weight() const { return 0; }
  };

  class LTLOp2: public Op2 {
    public:
      std::string str() const { return "LTLOp2"; }
      unsigned weight() const { return 0; }
  };

  class ProOp1: public Op1 {
    public:
      std::string str() const { return "ProOp1"; }
      unsigned weight() const { return 0; }
  };

  class ProOp2: public Op2 {
    public:
      std::string str() const { return "ProOp2"; }
      unsigned weight() const { return 0; }
  };

  class Next: public LTLOp1 {
    public:
      std::string str() const { return "X"; }
      unsigned weight() const { return 3; }
  };

  class Finally: public LTLOp1 {
    public:
      std::string str() const { return "F"; }
      unsigned weight() const { return 3; }
  };

  class Global: public LTLOp1 {
    public:
      std::string str() const { return "G"; }
      unsigned weight() const { return 3; }
  };

  class Until: public LTLOp2 {
    public:
      std::string str() const { return "U"; }
      unsigned weight() const { return 1; }
  };

  class Release: public LTLOp2 {
    public:
      std::string str() const { return "R"; }
      unsigned weight() const { return 1; }
  };

  class Not: public ProOp1 {
    public:
      std::string str() const { return "!"; }
      unsigned weight() const { return 3; }
  };

  class And: public ProOp2 {
    public:
      std::string str() const { return "&"; }
      unsigned weight() const { return 2; }
  };

  class Or: public ProOp2 {
    public:
      std::string str() const { return "|"; }
      unsigned weight() const { return 1; }
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
