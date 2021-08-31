#pragma once

#include <map>
#include <string>
#include <memory>
#include <set>

#include "literal.hpp"

namespace ltl {
  class Dict {
    public:
      std::map<std::string, std::shared_ptr<Literal>> map;
      std::shared_ptr<Literal> get(const Literal& li);
      std::shared_ptr<Literal> random_get();
      
      void set(const std::string&, const Literal& li);
      std::set<std::string> get_vocab() const;
  };

  extern Dict dict;
}
