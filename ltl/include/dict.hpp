#pragma once

#include <map>
#include <string>
#include <memory>

#include "literal.hpp"

namespace ltl {
  class Dict {
    public:
      std::map<std::string, std::shared_ptr<Literal>> map;
      std::shared_ptr<Literal> get(const Literal& li);
  };

  extern Dict dict;
}
