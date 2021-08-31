#include<random>

#include "ltl/dict.hpp"

static inline unsigned random_range(unsigned begin, unsigned end) {
  std::random_device r;
  std::default_random_engine e(r());
  std::uniform_int_distribution<unsigned> u(begin, end - 1);
  return u(e);
}

namespace ltl {
  std::shared_ptr<Literal> Dict::get(const Literal& li) {
    auto s = li.serialize();
    auto iter = this->map.find(s);
    if(iter == this->map.end()) {
      this->map[s] = std::make_shared<Literal>(li);
    }

    return this->map[s];
  }

  std::shared_ptr<Literal> Dict::random_get() {
    std::vector<std::shared_ptr<Literal>> vec_literals;
    for(auto& item: this->map) {
      if(item.second->serialize() != "0" && item.second->serialize() != "1") {
        vec_literals.push_back(item.second);
      }
    }
    auto idx = random_range(0, vec_literals.size());
    auto iter = vec_literals.begin() + idx;

    return *iter;
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
