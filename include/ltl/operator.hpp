#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <sstream>

namespace ltl {
  // 决定格式化的输出
  extern bool format_double_and; // true: &&, false: ||, default: false
  extern bool format_double_or;  // true: or, false: |, default: false
  extern bool format_symbol_F;   // true: <>, false: F, default: false
  extern bool format_symbol_G;   // true: [], false: G, default: false

  class Operator {
    public:
      friend std::ostream& operator<<(std::ostream& o, const Operator& ope);
      virtual std::string str() const = 0;
      virtual ~Operator() = default;
      virtual unsigned weight() const = 0;
      static std::shared_ptr<Operator> gen(const std::string& s);
      virtual bool can_reverse() const = 0;
  };

  class Op1: public Operator {
    public:
      std::string str() const { return "Op1"; }
      unsigned weight() const { return 0; }
      virtual bool can_reverse() const { return false; }
  };

  class Op2: public Operator {
    public:
      std::string str() const { return "Op2"; }
      unsigned weight() const { return 0; }
      virtual bool can_reverse() const { return false; }
  };

  class EmptyOp: public Op1 {
    public:
      std::string str() const { return "EmptyOp"; }
      unsigned weight() const { return 0; }
      virtual bool can_reverse() const { return false; }
  };

  class LTLOp1: public Op1 {
    public:
      std::string str() const { return "LTLOp1"; }
      unsigned weight() const { return 0; }
      virtual bool can_reverse() const { return false; }
  };

  class LTLOp2: public Op2 {
    public:
      std::string str() const { return "LTLOp2"; }
      unsigned weight() const { return 0; }
      virtual bool can_reverse() const { return false; }
  };

  class ProOp1: public Op1 {
    public:
      std::string str() const { return "ProOp1"; }
      unsigned weight() const { return 0; }
      virtual bool can_reverse() const { return false; }
  };

  class ProOp2: public Op2 {
    public:
      std::string str() const { return "ProOp2"; }
      unsigned weight() const { return 0; }
      virtual bool can_reverse() const { return false; }
  };

  class Next: public LTLOp1 {
    public:
      std::string str() const { return "X"; }
      unsigned weight() const { return 3; }
      virtual bool can_reverse() const { return false; }
  };

  class Finally: public LTLOp1 {
    public:
      std::string str() const { return format_symbol_F ? "<>": "F"; }
      unsigned weight() const { return 3; }
      virtual bool can_reverse() const { return false; }
  };

  class Global: public LTLOp1 {
    public:
      std::string str() const { return format_symbol_G ? "[]": "G"; }
      unsigned weight() const { return 3; }
      virtual bool can_reverse() const { return false; }
  };

  class Until: public LTLOp2 {
    public:
      std::string str() const { return "U"; }
      unsigned weight() const { return 1; }
      virtual bool can_reverse() const { return false; }
  };

  class Release: public LTLOp2 {
    public:
      std::string str() const { return "R"; }
      unsigned weight() const { return 1; }
      virtual bool can_reverse() const { return false; }
  };

  class Not: public ProOp1 {
    public:
      std::string str() const { return "!"; }
      unsigned weight() const { return 3; }
      virtual bool can_reverse() const { return false; }
  };

  class And: public ProOp2 {
    public:
      std::string str() const { return format_double_and ? "&&": "&"; }
      unsigned weight() const { return 2; }
      virtual bool can_reverse() const { return true; }
  };

  class Or: public ProOp2 {
    public:
      std::string str() const { return format_double_or ? "||": "|"; }
      unsigned weight() const { return 1; }
      virtual bool can_reverse() const { return true; }
  };

  class Imply: public ProOp2 {
    public:
      std::string str() const { return "->"; }
      unsigned weight() const { return 1; }
      virtual bool can_reverse() const { return false; }
  };

  class Equal: public ProOp2 {
    public:
      std::string str() const { return "<->"; }
      unsigned weight() const { return 1; }
      virtual bool can_reverse() const { return true; }
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
    extern std::shared_ptr<Imply> imply;
    extern std::shared_ptr<Equal> equal;
  }
}
