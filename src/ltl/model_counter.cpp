#include <algorithm>

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

std::ostream& ltl::operator<<(std::ostream& o, const BigInteger& bi) {
  o << bi.serialize();
  return o;
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
