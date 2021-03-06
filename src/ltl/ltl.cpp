#include <sstream>
#include <vector>
#include <string>
#include <stack>

#include "ltl/ltl.hpp"
#include "ltl/error.hpp"
#include "ltl/dict.hpp"

namespace ltl {
  std::string remove_blank(const std::string& _s) {
    std::string s = _s;
    auto iter = s.begin();
    while(iter != s.end()) {
      if(*iter == ' ' || *iter == '\r') {
        iter = s.erase(iter);
      } else {
        ++iter;
      }
    }
    return s;
  }

  std::vector<std::string> split_into_in_order(const std::string& s) {
    std::vector<std::string> specials = { "<->", "->", "&&", "||", "[]", "<>",
                                          "(", ")", "|", "&", "!", "~",
                                          "X", "G", "U", "R", "F" };
    // 分隔成vector
    std::vector<std::string> splits;

    // 从origin的at开始，尝试获得与pattern长度相同的字符，并且以std::string的形式返回
    auto get_string_with_same_size = [](const std::string& origin, unsigned at, const std::string& pattern) -> std::string {
      auto size = origin.size();
      auto pattern_size = pattern.size();
      if(at >= size) {
        return "";
      } else {
        return origin.substr(at, pattern_size);
      }
    };

    unsigned idx = 0;
    unsigned size = s.size();
    while(idx < size) {
      bool flag = false;
      for(const auto& special: specials) {
        auto detect = get_string_with_same_size(s, idx, special);
        if(detect == special) {
          splits.push_back(detect);
          idx += detect.size();
          flag = true;
          break;
        }
      }
      if(!flag) {
//        splits.emplace_back(std::string(1, s[idx]));
//        idx++;
        // s[idx]是一个普通的字符，找到这个普通字符的结尾，即直至遇到一个特殊字符，或遇到结尾
        auto end = idx + 1;
        auto is_special = false;
        while(end < size && !is_special) {
          for(auto& special: specials) {
            auto detect = get_string_with_same_size(s, end, special);
            if(detect == special) {
              is_special = true;
              break;
            }
          }
          if(!is_special) {
            ++end;
          }
        }

        splits.emplace_back(s.substr(idx, end - idx));
        idx = end;
      }
    }

    return splits;
  }

  std::vector<std::string> in_order_to_post_order(const std::vector<std::string>& inorder) {
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
        if(!matched) {
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
              while(!stack.empty() && stack.top() != "(") {
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
          // 判断是否使用imply
          if(!use_op_imply && dynamic_cast<Imply*>(op.get()) != nullptr) {
            auto not_b = std::make_shared<LTL::LTLNode>(op::nnot, b);
            stack.push(std::make_shared<LTL::LTLNode>(not_b, op::oor, a));
          } else {
            stack.push(std::make_shared<LTL::LTLNode>(b, op, a));
          }
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

  bool LTL::LTLNode::is_literal() const {
    return this->op.get() == nullptr;
  }

  bool LTL::LTLNode::is_literal_negative() const {
    return this->op == op::nnot && this->right->is_literal();
  }

  bool LTL::LTLNode::is_op1() const {
    if(!this->op) {
      return false;
    } else {
      if(dynamic_cast<Op1*>(this->op.get()) != nullptr) {
        return true;
      } else {
        return false;
      }
    }
  }

  bool LTL::LTLNode::is_op2() const {
    if(!this->op) {
      return false;
    } else {
      if(dynamic_cast<Op2*>(this->op.get()) != nullptr) {
        return true;
      } else {
        return false;
      }
    }
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

      auto left = this->left->serialize();
      auto right = this->right->serialize();

      if(this->op->can_reverse()) {
        bool left_first = false;
        if(this->left->is_literal() || this->left->is_literal_negative()) {
          left_first = true;
        } else {
          auto left_op = this->left->op.get();
          if(dynamic_cast<ProOp1*>(left_op) || dynamic_cast<ProOp2*>(left_op)) {
            left_first = true;
          }
        }
        if(left_first == false) {
          std::string tmp = std::move(left);
          left = std::move(right);
          right = std::move(tmp);
        }

        ostr << "(";
        ostr << left;
        ostr << ")";
        ostr << this->op->str();
        ostr << "(";
        ostr << right;
        ostr << ")";
      } else {
        ostr << "(";
        ostr << left;
        ostr << ")";
        ostr << this->op->str();
        ostr << "(";
        ostr << right;
        ostr << ")";
      }
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
    // return this->serialize() < other.serialize();
    auto this_str = this->serialize();
    auto other_str = other.serialize();
    if(this_str.size() < other_str.size()) {
      return true;
    } else if(this_str.size() == other_str.size()) {
      return this_str < other_str;
    } else {
      return false;
    }
  }

  bool LTL::operator==(const LTL& other) const {
    auto this_str = this->serialize();
    auto other_str = other.serialize();
    return this_str == other_str;
  }

  bool LTL::operator!=(const LTL& other) const {
    return !(*this == other);
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
  ops2(imply)

  #undef ops2
  #undef ops1

  unsigned LTL::depth() const {
    if(!this->root) {
      return 0;
    } else {
      return this->root->depth();
    }
  }

  std::vector<LTL::LTLNode*>
    LTL::get_level_order() {
    std::vector<LTL::LTLNode*> ret = this->root->get_level_order();

    return ret;
  }

  bool LTL::is_boolean_formula() const {
    return this->root->is_boolean_formula();
  }
}
