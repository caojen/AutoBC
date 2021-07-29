#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <stack>

#include "ltl.hpp"
#include "error.hpp"
#include "dict.hpp"

namespace ltl {
  static std::string remove_blank(const std::string& _s) {
    std::string s = _s;
    auto iter = s.begin();
    while(iter != s.end()) {
      if(*iter == ' ') {
        iter = s.erase(iter);
      } else {
        ++iter;
      }
    }
    return s;
  }

  static std::vector<std::string> split_into_in_order(const std::string& s) {
    std::string special = "()|&!";
    std::string pre = "XGURF";
    // 分隔成vector
    std::vector<std::string> splits;
    for(unsigned i = 0; i < s.size();) {
      std::string str = std::string(1, s[i]);
      if(special.find(s[i]) != std::string::npos) {
        splits.push_back(std::string(1, s[i]));
        i++;
        continue;
      } else if(pre.find(s[i]) != std::string::npos) {
        unsigned j = i + 1;
        bool is_special = j < s.size();
        while(j < s.size() && is_special) {
          if(special.find(s[j]) != std::string::npos) {
            break;
          }
          if(pre.find(s[j]) != std::string::npos) {
            if(j + 1 == s.size()) {
              is_special = false;
            }
            j++;
          } else {
            break;
          }
        }
        if(is_special) {
          splits.push_back(std::string(1, s[i]));
          i++;
        } else {
          goto not_special;
        }
      } else {
not_special:
        unsigned j = i;
        while(j < s.size() && special.find(s[j]) == std::string::npos) {
          ++j;
        }
        splits.push_back(s.substr(i, j-i));
        i = j;
      }
    }

    return splits;
  }

  static std::vector<std::string> in_order_to_post_order(const std::vector<std::string>& inorder) {
    std::stack<std::string> stack;
    std::vector<std::string> ret;

    for(auto& ch: inorder) {
      if(ch == "(") {
        stack.push("(");
      } else if(ch == ")") {
        bool matched = false;
        while(!stack.empty()) {
          std::string t = std::move(stack.top()); stack.pop();
          if(t != "(") {
            ret.push_back(std::move(t));
          } else {
            matched = true;
            break;
          }
        }
        if(matched == false) {
          throw not_a_ltl();
        }
      } else {
        auto op = Operator::gen(ch);
        if(op == op::emptyOp) {
          // 不是一个操作符
          ret.push_back(ch);
        } else {
          // 是一个操作符
          if(stack.empty()) {
            stack.push(ch);
          } else {
            auto last = Operator::gen(stack.top());
            if(last->weight() <= op->weight()) {
              stack.push(ch);
            } else {
              while(stack.empty() == false && stack.top() != "(") {
                auto last = Operator::gen(stack.top());
                if(last->weight() <= op->weight()) {
                  break;
                } else {
                  ret.push_back(stack.top());
                  stack.pop();
                }
              }
              stack.push(ch);
            }
          }
        }
      }
    }

    while(!stack.empty()) {
      auto item = stack.top(); stack.pop();
      if(item == "(") {
        throw not_a_ltl();
      } else {
        ret.push_back(item);
      }
    }

    return ret;
  }

  static LTL gen_from_post_order(const std::vector<std::string>& postorder) {
    // 使用LTLNode而不是LTL来减少拷贝构造函数的深复制次数
    std::stack<std::shared_ptr<LTL::LTLNode>> stack;
    for(auto& s: postorder) {
      // 如果s是个Operator:
      auto op = Operator::gen(s);
      if(op != op::emptyOp) {
        // 判断s是几元运算符
        if(dynamic_cast<Op1*>(op.get()) != nullptr) {
          // 一元运算符
          // 尝试从stack中获取一个操作数
          if(stack.empty()) {
            throw not_a_ltl();
          }
          std::shared_ptr<LTL::LTLNode> a = stack.top();
          stack.pop();
          stack.push(std::make_shared<LTL::LTLNode>(op, a));
        } else if(dynamic_cast<Op2*>(op.get()) != nullptr) {
          // 二元运算符
          // 尝试从stack中获取两个操作数
          if(stack.size() < 2) {
            throw not_a_ltl();
          }
          std::shared_ptr<LTL::LTLNode> a = stack.top();
          stack.pop();
          std::shared_ptr<LTL::LTLNode> b = stack.top();
          stack.pop();
          stack.push(std::make_shared<LTL::LTLNode>(b, op, a));
        } else {
          throw unreachable();
        }
      } else {
        // op是一个符号（一个文字），直接入栈
        stack.push(std::make_shared<LTL::LTLNode>(Literal(s)));
      }
    }
    if(stack.size() != 1) {
      throw not_a_ltl();
    }

    LTL ret;
    ret.root = stack.top();
    return ret;
  }

  LTLGenerator::LTLGenerator() {
    this->preOps.clear();
    this->li = "";
    this->left = nullptr;
    this->mid = nullptr;
    this->right = nullptr;
  }

  LTLGenerator::LTLGenerator(const Literal& li) {
    this->preOps.clear();
    this->li = li;
    this->left = nullptr;
    this->mid = nullptr;
    this->right = nullptr;
  }

  LTLGenerator::LTLGenerator(const LTLGenerator& pro) {
    this->preOps = pro.preOps;
    this->li = pro.li;
    this->left = pro.left;
    this->mid = pro.mid;
    this->right = pro.right;
  }

  LTLGenerator LTLGenerator::next() const {
    if(this->empty()) {
      throw not_a_ltl();
    }
    LTLGenerator ret = *this;
    ret.preOps.push_front(op::next);

    return ret;
  }

  LTLGenerator LTLGenerator::finally() const {
    if(this->empty()) {
      throw not_a_ltl();
    }

    LTLGenerator ret = *this;
    ret.preOps.push_front(op::finally);

    return ret;
  }

  LTLGenerator LTLGenerator::global() const {
    if(this->empty()) {
      throw not_a_ltl();
    }

    LTLGenerator ret = *this;
    ret.preOps.push_front(op::global);

    return ret;
  }

  LTLGenerator LTLGenerator::nnot() const {
    if(this->empty()) {
      throw not_a_ltl();
    }

    LTLGenerator ret = *this;
    ret.preOps.push_front(op::nnot);

    return ret;
  }

  LTLGenerator LTLGenerator::until(const LTLGenerator& ltl) const {
    if(this->empty()) {
      throw not_a_ltl();
    }

    std::shared_ptr<LTLGenerator> left(new LTLGenerator(*this));
    std::shared_ptr<Op2> mid = op::until;
    std::shared_ptr<LTLGenerator> right(new LTLGenerator(ltl));

    LTLGenerator ret;
    ret.left = left;
    ret.mid = mid;
    ret.right = right;

    return ret;
  }

  LTLGenerator LTLGenerator::release(const LTLGenerator& ltl) const {
    if(this->empty()) {
      throw not_a_ltl();
    }

    std::shared_ptr<LTLGenerator> left(new LTLGenerator(*this));
    std::shared_ptr<Op2> mid = op::release;
    std::shared_ptr<LTLGenerator> right(new LTLGenerator(ltl));

    LTLGenerator ret;
    ret.left = left;
    ret.mid = mid;
    ret.right = right;

    return ret;
  }

  LTLGenerator LTLGenerator::aand(const LTLGenerator& ltl) const {
    if(this->empty()) {
      throw not_a_ltl();
    }

    std::shared_ptr<LTLGenerator> left(new LTLGenerator(*this));
    std::shared_ptr<Op2> mid = op::aand;
    std::shared_ptr<LTLGenerator> right(new LTLGenerator(ltl));

    LTLGenerator ret;
    ret.left = left;
    ret.mid = mid;
    ret.right = right;

    return ret;
  }

  LTLGenerator LTLGenerator::oor(const LTLGenerator& ltl) const {
    if(this->empty()) {
      throw not_a_ltl();
    }

    std::shared_ptr<LTLGenerator> left(new LTLGenerator(*this));
    std::shared_ptr<Op2> mid = op::oor;
    std::shared_ptr<LTLGenerator> right(new LTLGenerator(ltl));

    LTLGenerator ret;
    ret.left = left;
    ret.mid = mid;
    ret.right = right;

    return ret;
  }

  LTLGenerator::~LTLGenerator() {}

  std::ostream& operator<<(std::ostream& o, const LTLGenerator& ltl) {
    o << ltl.serialize();

    return o;
  }

  std::string LTLGenerator::serialize() const {
    std::ostringstream o;

    // 输出所有前缀运算符
    for(auto& op: this->preOps) {
      o << op->str();
    }

    o << "(";

    // 判断这是不是一个文字，如果是，那么直接输出这个文字
    if(this->singal()) {
      o << this->li;
    } else {
      // 递归地输出第一部分:
      o << this->left->serialize();
      // 如果存在第二部分:
      if(this->mid != nullptr) {
        // 输出中间的运算符
        o << this->mid->str();
        if(this->right != nullptr) {
          // 递归地输出第二部分
          o << this->right->serialize();
        }
      }
    }
    o << ")";
    std::string ret = o.str();
    // 清除中的所有空格
    auto iter = ret.begin();
    while(iter != ret.end()) {
      if(*iter == ' ') {
        iter = ret.erase(iter);
      } else {
        ++iter;
      }
    }

    return ret;
  }

  bool LTLGenerator::operator==(const LTLGenerator& other) const {
    return this->serialize() == other.serialize();
  }

  bool LTLGenerator::empty() const {
    return this->li == "" && this->left &&
      this->mid && this->right;
  }

  bool LTLGenerator::singal() const {
    return !this->empty() && this->li != "";
  }

  LTLGenerator LTLGenerator::Gen(const LTLGenerator& ltl) {
    return ltl;
  }

  LTL LTLGenerator::parse(const std::string& s) {
    return LTL::parse(s);
  }

  bool LTL::LTLNode::is_literal() const {
    return this->op.get() == nullptr;
  }

  bool LTL::LTLNode::is_op1() const {
    return this->op == op::finally ||
      this->op == op::global ||
      this->op == op::next ||
      this->op == op::nnot;
  }

  bool LTL::LTLNode::is_op2() const {
    return this->op == op::aand ||
      this->op == op::oor ||
      this->op == op::release ||
      this->op == op::until;
  }

  std::ostream& operator<<(std::ostream& o, const LTL::LTLNode& ltlNode) {
    o << ltlNode.serialize();
    return o;
  }

  std::string LTL::LTLNode::serialize() const {
    std::ostringstream ostr("");

    if(this->is_literal()) {
      ostr << this->literal->serialize();
    } else if(this->is_op1()) {
      ostr << this->op->str();
      ostr << "(";
      ostr << this->right->serialize();
      ostr << ")";
    } else if(this->is_op2()) {
      ostr << "(";
      ostr << this->left->serialize();
      ostr << ")";
      ostr << this->op->str();
      ostr << "(";
      ostr << this->right->serialize();
      ostr << ")";
    } else {
      throw not_a_ltl();
    }
    return ostr.str();
  }

  std::ostream& operator<<(std::ostream& o, const LTL& ltl) {
    o << ltl.serialize();
    return o;
  }

  std::string LTL::serialize() const {
    return this->root->serialize();
  }

  LTL LTL::parse(const std::string& _s) {
    // 删除s中所有的空格
    std::string s = remove_blank(_s);
    
    std::vector<std::string> inorder = split_into_in_order(s);
    // inorder 是一个中缀表达式，将其转为后缀表达式

    std::vector<std::string> postorder = in_order_to_post_order(inorder);

    // 根据后缀表达式构造LTL树
    
    return gen_from_post_order(postorder);
  }

  bool LTL::operator<(const LTL& other) const {
    return this->serialize() < other.serialize();
  }

  #define ops1(ops) LTL LTL::ops() const {    \
    LTL ret;                                  \
    ret.root = std::make_shared<LTLNode>(     \
      op::ops,                                \
      this->root                              \
    );                                        \
    return ret;                               \
  }

  #define ops2(ops) LTL LTL::ops              \
      (const LTL& other) const {              \
    LTL ret;                                  \
    ret.root = std::make_shared<LTLNode>(     \
      this->root,                             \
      op::ops,                                \
      other.root                              \
    );                                        \
    return ret;                               \
  }

  ops2(aand)
  ops2(oor)
  ops1(nnot)
  ops1(global)
  ops1(next)
  ops1(finally)
  ops2(release)
  ops2(until)

  #undef ops2
  #undef ops1
}
