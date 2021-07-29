#include "dict.hpp"

namespace ltl {
  std::shared_ptr<Literal> Dict::get(const Literal& li) {
    auto s = li.serialize();
    auto iter = this->map.find(s);
    if(iter == this->map.end()) {
      this->map[s] = std::make_shared<Literal>(li);
    }

    return this->map[s];
  }

  Dict dict;
}
