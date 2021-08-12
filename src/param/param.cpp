#include "param.hpp"
#include "param_error.hpp"

namespace param {
  void Param::set(
    const std::string& brief,         // 例如， -s
    const std::string& detail,        // 例如， --start
    const std::string& key,           // 如何查找这个关键字，例如: start
    bool after,                       // 这个符号后面是否需要额外添加字符，true时例如 -s 100, false时后面省略100
    bool required,                    // 判断这个符号是否为必须的,
    const std::string& description,   // 这个符号的帮助说明
    const std::string& def            // 这个字段的默认数值，如果after为true，那么按照这个默认值来填；如果after为false，那么将会丢弃这个def字段（即使给定），使用"false"字符串
  ) {
    this->briefs[brief] = key;
    this->details[detail] = key;
    this->keys.insert(key);
    this->afters[key] = after;
    this->requireds[key] = required;
    this->descriptions[key] = description;
    if(after) {
      this->defs[key] = def;
    } else {
      this->defs[key] = "false";
    }
  }

  void Param::run(int argc, char** argv) {
    for(int i = 1; i < argc; i++) {
      auto key = std::string(argv[i]);
      if(keys.find(key) == keys.end()) {
        throw no_such_key(key);
      }
      if(afters[key]) {
        if(i + 1 >= argc) {
          throw no_value_provided();
        }
        auto value = std::string(argv[i + 1]);
        ++i;
        values[key] = value;
      } else {
        values[key] = "true";
      }
    }
    for(auto iter = values.begin(); iter != values.end(); ++iter) {
      requireds[iter->first] = false;
    }

    for(auto iter = requireds.begin(); iter != requireds.end(); ++iter) {
      if(iter->second) {
        throw required_key_not_provided(iter->first);
      }
    }
  }

  std::string Param::get(const std::string& key) const {
    if(this->keys.find(key) == this->keys.end()) {
      throw no_such_key(key);
    }

    auto iter = values.find(key);
    if(iter != values.end()) {
      return iter->second;
    }

    if(this->afters.find(key) == this->afters.end()) {
      return "false";
    } else {
      return this->defs.find(key)->second;
    }
  }

  std::string Param::operator[](const std::string& key) const {
    return this->get(key);
  }
}
