#include "ltl/dict.hpp"

namespace ltl {
  std::shared_ptr<Literal> Dict::get(const Literal& li) {
    auto s = li.serialize();
    auto iter = this->map.find(s);
    if(iter == this->map.end()) {
      this->map[s] = std::make_shared<Literal>(li);
    }

    return this->map[s];
  }

  void Dict::set(const std::string& s, const Literal &li) {
    auto iter = this->map.find(s);
    if(iter != this->map.end()) {
      iter->second->set(li);
    } else {
      this->map[s] = std::make_shared<Literal>(li);
    }
  }

  std::set<std::string> Dict::get_vocab() const {
    std::set<std::string> ret;
    for(auto& s: this->map) {
      if(s.first == "1" || s.first == "0") {
        ret.insert("a");
      } else {
        ret.insert(s.first);
      }
    }
    return ret;
  }


  Dict dict;
}
