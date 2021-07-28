#include "dict.hpp"

namespace autobc {
  std::shared_ptr<Literal> Dict::get(const Literal& li) {
    auto iter = this->map.find(li);
    if(iter == this->map.end()) {
      this->map[li] = std::make_shared<Literal>(li);
    }

    return this->map[li];
  }
}
