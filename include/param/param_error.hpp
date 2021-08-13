#pragma once

#include <exception>
#include <string>

namespace param {
  class no_such_key: public std::exception {
    private:
      std::string key;
    public:
      no_such_key(const std::string& key) {
        this->key = key;
      }

      virtual const char* what() noexcept {
        std::string ret = this->key + " doesnot exists.";
        auto chars = ret.c_str();
        return chars;
      }
  };

  class no_value_provided: public std::exception {
    public:
      virtual const char* what() noexcept {
        return "no value provided";
      }
  };

  class required_key_not_provided: public std::exception {
    private:
      std::string key;
    public:
      required_key_not_provided(const std::string& key) {
        this->key = key;
      }
      virtual const char* what() noexcept {
        std::string ret = "required_key_not_provided: ";
        ret.append(key);
        return ret.c_str();
      }
  };
}