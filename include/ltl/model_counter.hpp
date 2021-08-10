#pragma once

#include <string>
#include <iostream>
#include <set>
#include "ltl.hpp"

namespace ltl {
  class LTL;
  class BigInteger {
  public:
      BigInteger() = default;
      BigInteger(long long);
      // 字符串格式化时，要么全部是数字，要么除了第一个字符外是'+'或'-'而其余全是数字
      // 否则，抛出not_an_integer错误
      BigInteger(const std::string&);

      BigInteger(const BigInteger&) = default;

      std::string serialize() const;
      friend std::ostream& operator<<(std::ostream& o, const BigInteger& bi);

      // 重载加减法，比较运算符
      BigInteger operator+(const BigInteger& other) const;
      BigInteger operator-(const BigInteger& other) const;
      bool operator==(const BigInteger& other) const;
      bool operator!=(const BigInteger& other) const;
      bool operator<(const BigInteger& other) const;
      bool operator>(const BigInteger& other) const;
  private:
      bool        op          = true ;     // 当op为true是表示是正数，否则是负数
      std::string data       = "0"  ;     // 保留绝对值的字符串形式
  };

  class ModelCounter {
  public:
      ModelCounter(const std::string& counter = "../jar/modelcounting.jar", const std::string& javapath = "/usr/bin/java");

      BigInteger count(const std::set<LTL>& ltls, unsigned bound);

      std::string     counter;
      std::string     javapath;           // java的路径，ModelCounter的jdk规定为16
  };
}
