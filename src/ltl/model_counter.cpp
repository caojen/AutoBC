#include <algorithm>
#include <unistd.h>
#include <array>
#include <sys/wait.h>
#include <cstring>

#include "model_counter.hpp"
#include "error.hpp"

using namespace ltl;

BigInteger::BigInteger(long long num) {
  if(num == 0) {
    op = true;
    data = "0";
    return;
  }

  if(num < 0) {
    num = -num;
    op = false;
  }

  data = "";
  while(num) {
    data.append(std::string(1, static_cast<char>(num % 10 + '0')));
    num /= 10;
  }
}

BigInteger::BigInteger(const std::string & s) {
  unsigned idx = 0;
  if(s.empty()) {
    return;
  }
  if(s.at(idx) == '+') {
    op = true;
    ++idx;
  } else if(s.at(idx) == '-') {
    op = false;
    ++idx;
  }

  auto size = s.size();
  auto i = idx;
  while(i < size) {
    if(s.at(i) < '0' || s.at(i) > '9') {
      throw not_an_integer();
    }
    ++i;
  }

  data = s.substr(idx);
}

std::string BigInteger::serialize() const {
  std::string ret;
  if(!op) {
    ret.append("-");
  }
  ret.append(data);
  return ret;
}

BigInteger BigInteger::operator+(const BigInteger& other) const {
  BigInteger ret;
  if(this->op == other.op) {
    ret.op = this->op;
    ret.data = "";
    auto left = this->data.rbegin();
    auto right = other.data.rbegin();
    unsigned carry = 0;
    while(left != this->data.rend() || right != other.data.rend() || carry) {
      unsigned sum = 0;
      if(left != this->data.rend()) {
        sum += static_cast<char>(*left) - '0';
        ++left;
      }
      if(right != other.data.rend()) {
        sum += static_cast<char>(*right) - '0';
        ++right;
      }
      sum += carry;

      if(sum > 9) {
        sum -= 10;
        carry = 1;
      } else {
        carry = 0;
      }
      ret.data.append(std::string(1, static_cast<char>('0' + sum)));
    }
    std::reverse(ret.data.begin(), ret.data.end());
  } else {
    if(this->op) {
      // 正 + 负
      BigInteger cp = other;
      cp.op = true;
      ret = (*this) - cp;
    } else {
      // 负 + 正
      BigInteger cp = *this;
      cp.op = true;
      ret = other - cp;
    }
  }

  return ret;
}

BigInteger BigInteger::operator-(const BigInteger& other) const {
  BigInteger ret;
  // 首先判断符号
  if(this->op && other.op) { // 两个都是正数
    // 正数减去正数
    unsigned lsize = this->data.size();
    unsigned rsize = other.data.size();
    if(lsize == rsize) {
      ret.op = this->data >= other.data;
    } else if(lsize > rsize) {
      ret.op = true;
    } else {
      ret.op = false;
    }
    if(ret.op) {
      ret.data = "";
      auto left = this->data.rbegin();
      auto right = other.data.rbegin();
      unsigned carry = 0;
      while(left != this->data.rend() || right != other.data.rend() || carry) {
        signed r = 0;
        if(left != this->data.rend()) {
          r += static_cast<char>(*left) - '0';
          ++left;
        }
        if(right != other.data.rend()) {
          r -= static_cast<char>(*right) - '0';
          ++right;
        }
        r -= (signed)carry;
        if(r < 0) {
          r += 10;
          carry = 1;
        } else {
          carry = 0;
        }
        ret.data.append(std::string(1, static_cast<char>(r + '0')));
      }
      std::reverse(ret.data.begin(), ret.data.end());
    } else {
      ret = other - (*this);
    }
  } else if(this->op && !other.op) {
    // 正数减去负数
    BigInteger cp = other;
    cp.op = true;
    ret = (*this) + cp;
  } else if(!this->op && other.op) {
    // 负数减去正数
    BigInteger cp = other;
    cp.op = false;
    ret = (*this) + cp;
  } else {
    // 负数减去负数
    auto lsize = this->data.size();
    auto rsize = other.data.size();
    if(lsize == rsize) {
      ret.op = this->data < other.data;
    } else if(lsize > rsize) {
      ret.op = false;
    } else {
      ret.op = true;
    }

    if(ret.op) {
      BigInteger cp = other;
      cp.op = true;
      ret = cp - (*this);
    } else {
      ret.data = "";
      auto left = this->data.rbegin();
      auto right = other.data.rbegin();
      unsigned carry = 0;
      while(left != this->data.rend() || right != other.data.rend() || carry) {
        signed r = 0;
        if(left != this->data.rend()) {
          r += static_cast<char>(*left) - '0';
          ++left;
        }
        if(right != other.data.rend()) {
          r -= static_cast<char>(*right) - '0';
          ++right;
        }
        r -= (signed)carry;
        if(r < 0) {
          r += 10;
          carry = 1;
        } else {
          carry = 0;
        }
        ret.data.append(std::string(1, static_cast<char>(r + '0')));
      }
      std::reverse(ret.data.begin(), ret.data.end());
    }
  }
  // 删去可能出现的前导0，除非ret是0
  unsigned idx = 0;
  while(idx < ret.data.size() && ret.data.at(idx) == '0') {
    ++idx;
  }
  if(idx < ret.data.size()) {
    ret.data = ret.data.substr(idx);
  } else {
    ret.op = true;
    ret.data = "0";
  }
  return ret;
}

bool BigInteger::operator==(const BigInteger& other) const {
  return this->op == other.op && this->data == other.data;
}
bool BigInteger::operator!=(const BigInteger& other) const {
  return !((*this) == other);
}
bool BigInteger::operator<(const BigInteger& other) const {
  if(!this->op && other.op) {
    return true;
  } else {
    auto r = (*this) - other;
    return !r.op;
  }
}
bool BigInteger::operator>(const BigInteger& other) const {
  return (*this) != other && !((*this) < other);
}

ModelCounter::ModelCounter(const std::string &counter, const std::string& javapath) {
  this->counter = counter;
  this->javapath = javapath;
}

BigInteger ModelCounter::count(const std::set<LTL> &ltls, unsigned int bound) {
  auto format_double_and = ltl::format_double_and;
  auto format_double_or = ltl::format_double_or;
  auto format_symbol_F = ltl::format_symbol_F;
  auto format_symbol_G = ltl::format_symbol_G;
  ltl::format_double_and = true;
  ltl::format_double_or = true;
  ltl::format_symbol_F = false;
  ltl::format_symbol_G = false;

  auto iter = ltls.begin();
  LTL ltl;
  while(iter != ltls.end()) {
    if(ltl.root == nullptr) {
      ltl = *iter;
    } else {
      ltl = ltl.aand(*iter);
    }
    ++iter;
  }
  auto f = ltl.serialize();
  ltl::format_double_and = format_double_and;
  ltl::format_double_or = format_double_or;
  ltl::format_symbol_F = format_symbol_F;
  ltl::format_symbol_G = format_symbol_G;
  auto k = bound;
  std::vector<std::string> vars;
  for(auto & kv : ltl::dict.map) {
    vars.push_back(kv.first);
  }
  std::string vars_str;
  for(unsigned i = 0; i < vars.size(); i++) {
    vars_str.append(vars[i]);
    if(i != vars.size() - 1) {
      vars_str.append(",");
    }
  }
  std::array<std::string, 6> args = {
          "java",
          "-jar",
          this->counter,
          "-k=" + std::to_string(k),
          "-vars=" + vars_str,
          "-f=" + f
  };

  int fd[2];
  if(pipe(fd)) {
    std::cout << "Fatal: Cannot create pipe(use system call 'pipe()' failed.)" << std::endl;
    std::cout << strerror(errno) << std::endl;
  }

  int pid = fork();
  if(pid == 0) {
    // child, call java
    // 写入fd[1]
    dup2(fd[1], 1);
    dup2(fd[1], 2);
    close(fd[1]);

    char* nargs[7] = { nullptr };
    for(unsigned i = 0; i < 6; i++) {
      nargs[i] = new char[args[i].size() + 1];
      auto s = args[i].c_str();
      memcpy(nargs[i], s, args[i].size() * sizeof(char));
      nargs[i][args[i].size()] = 0;
    }

    auto ret = execv(this->javapath.c_str(), nargs);
    std::cout << "fatal: child returned: " << ret << std::endl;
    std::cout << strerror(errno) << std::endl;
    exit(1);
  } else if(pid < 0) {
    std::cout << "fork failed" << std::endl;
    exit(1);
  } else if(pid > 0) {
    waitpid(pid, nullptr, 0);
  }
  std::string result;
  char buf[1024] = { 0 };
  close(fd[1]);
  while(read(fd[0], buf, 1024)) {
    result.append(buf);
  }
  close(fd[0]);
  result = result.substr(0, result.size() - 1);
  return { result };
}
