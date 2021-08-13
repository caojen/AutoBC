#include <sstream>

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
    this->name = std::string(argv[0]);
    for(int i = 1; i < argc; i++) {
      auto target = std::string(argv[i]);
      if(target == "-h" || target == "--help") {
        std::cout << get_help_page() << std::endl;
        exit(0);
      }
      std::string key;
      if(briefs.find(target) != briefs.end()) {
        key = briefs[target];
      } else if(details.find(target) != details.end()) {
        key = details[target];
      } else {
        std::cout << get_help_page() << std::endl;
        throw no_such_key(target);
      }
      if(keys.find(key) == keys.end()) {
        std::cout << get_help_page() << std::endl;
        throw no_such_key(key);
      }
      if(afters[key]) {
        if(i + 1 >= argc) {
          std::cout << get_help_page() << std::endl;
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
        std::cout << get_help_page() << std::endl;
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

  std::string Param::get_help_page() const {
    auto find_key_by_value = [](const std::map<std::string, std::string>& map, const std::string& value) -> std::string {
      for(auto iter = map.begin(); iter != map.end(); ++iter) {
        if(iter->second == value) {
          return iter->first;
        }
      }

      return "";
    };
    std::ostringstream ostr;
    ostr << "Usage: " << this->name << " ";
    for(auto& key: this->keys) {
      auto brief = find_key_by_value(this->briefs, key);
      auto detail = find_key_by_value(this->details, key);
      auto required = this->requireds.find(key)->second;
      auto after = this->afters.find(key)->second;
      auto description = this->descriptions.find(key)->second;
      if(required) {
        ostr << brief << "|" << detail << " ";
      } else {
        ostr << "[" << brief << "|" << detail << "] ";
      }
      if(after) {
        ostr << "${" << key << "} ";
      }
    }
    ostr << std::endl;
    ostr << std::endl;
    ostr << "-h" << ", " << "--help" << " : " << "Show Help Page." << std::endl;
    for(auto& key: this->keys) {
      auto brief = find_key_by_value(this->briefs, key);
      auto detail = find_key_by_value(this->details, key);
      auto required = this->requireds.find(key)->second;
      auto after = this->afters.find(key)->second;
      auto description = this->descriptions.find(key)->second;
      ostr << brief << ", " << detail << " ";
      if(after) {
        ostr << key << " ";
      }
      ostr << std::endl << "\t\t\t";
      if(required) {
        ostr << "[required] ";
      }
      ostr << description << std::endl;
    }

    return ostr.str();
  }
}
