#pragma once

#include <iostream>
#include <deque>
#include <list>
#include <memory>
#include <string>
#include <map>
#include <stack>
#include <vector>
#include <queue>

#include "literal.hpp"
#include "operator.hpp"
#include "dict.hpp"
#include "error.hpp"
#include "sat.hpp"
#include "model_counter.hpp"

namespace ltl{
  class LTL {
    public:
      class LTLNode {
        public:
          // 当op为EmptyOp时，该节点为一个literal，left和right无效
          // 当op为Op1时，该节点的right有效，literal和left无效
          // 否则（op为Op2），该节点的left和right有效，literal无效
          std::shared_ptr<Operator>           op;
          std::shared_ptr<Literal>            literal;
          std::shared_ptr<LTLNode>            left;
          std::shared_ptr<LTLNode>            right;

          bool is_literal() const;
          bool is_literal_negative() const;
          bool is_op1() const;
          bool is_op2() const;

          friend std::ostream& operator<<(std::ostream& o, const LTLNode& ltlNode);
          std::string serialize() const;

          // 构造函数
          LTLNode() {}
          // 该LTLNode是一个文字
          LTLNode(const Literal& literal) {
            this->literal = dict.get(literal);
          }
          // 该LTL是一个一元运算符控制的，深复制right而不改变其指针的内容
          LTLNode(std::shared_ptr<Operator> op, const std::shared_ptr<LTLNode>& right) {
            this->op = op;
            this->right = std::make_shared<LTLNode>(*right);
          }
          // 该LTL是一个二元运算符控制的，深复制left和right而不改变其指针的内容
          LTLNode(const std::shared_ptr<LTLNode>& left, std::shared_ptr<Operator> op, const std::shared_ptr<LTLNode>& right) {
            this->left = std::make_shared<LTLNode>(*left);
            this->op = op;
            this->right = std::make_shared<LTLNode>(*right);
          }
          // 深复制拷贝构造函数
          LTLNode(const LTLNode& other) {
            if(other.op) {
              this->op = other.op;
            }
            if(other.literal) {
              this->literal = other.literal;
            }
            if(other.left) {
              this->left = std::make_shared<LTLNode>(*other.left);
            }
            if(other.right) {
              this->right = std::make_shared<LTLNode>(*other.right);
            }
          }

          unsigned depth() const {
            if(this->is_literal()) {
              return 1;
            } else if(this->is_op1()) {
              return 1 + this->right->depth();
            } else if(this->is_op2()) {
              auto nleft = this->left->depth();
              auto nright = this->right->depth();
              return 1 + nleft + nright;
            } else {
              throw ltl::unreachable();
            }
          }

          std::vector<LTLNode*> get_level_order() {
            std::vector<LTLNode*> ret;

            std::queue<LTLNode*> queue;
            queue.push(this);
            while(!queue.empty()) {
              auto node = queue.front(); queue.pop();
              ret.push_back(node);
              if(node->is_op1()) {
                queue.push(node->right.get());
              } else if(node->is_op2()) {
                queue.push(node->left.get());
                queue.push(node->right.get());
              } else if(!node->is_literal()) {
                throw unreachable();
              }
            }
            return ret;
          };

          bool is_boolean_formula() const {
            if(this->is_literal()) {
              return true;
            } else if(this->is_op1()) {
              if(dynamic_cast<ProOp1*>(this->op.get()) != nullptr) {
                return this->right->is_boolean_formula();
              } else {
                return false;
              }
            } else if(this->is_op2()) {
              if(dynamic_cast<ProOp2*>(this->op.get()) != nullptr) {
                return this->left->is_boolean_formula() && this->right->is_boolean_formula();
              } else {
                return false;
              }
            } else {
              throw not_a_ltl();
            }
          }
      };

      LTL() = default;

      // 深复制拷贝构造函数
      LTL(const LTL& ltl) {
        this->root = std::make_shared<LTLNode>(*ltl.root);
      }

      LTL(const LTLNode& node) {
        this->root = std::make_shared<LTLNode>(node);
      }

      LTL(const std::shared_ptr<LTLNode>& ptr) {
        this->root = std::make_shared<LTLNode>(*ptr);
      }

      // 移动构造函数
//      LTL(LTL&& ltl) noexcept {
//        this->root = ltl.root;
//      }

      // 保存LTLNode的指针
      std::shared_ptr<LTLNode> root;

      // 格式化与流输出
      friend std::ostream& operator<<(std::ostream& o, const LTL& ltl);
      std::string serialize() const;

      // 根据字符串来返回对应的LTL公式
      // 错误时抛出not_a_ltl错误
      static LTL parse(const std::string& s);

      // 使LTL产生偏序
      bool operator<(const LTL& other) const;
      bool operator==(const LTL& other) const;
      bool operator!=(const LTL& other) const;

      // 提供LTL的直接运算接口。这类运算会进行深拷贝而不影响已有的
      LTL aand(const LTL& other) const;
      LTL oor(const LTL& other) const;
      LTL nnot() const;
      LTL global() const;
      LTL next() const;
      LTL finally() const;
      LTL release(const LTL& other) const;
      LTL until(const LTL& other) const;
      LTL imply(const LTL& other) const;

      // 计算这个LTL深度
      unsigned depth() const;

      // 获取这个LTL的层序遍历的LTLNode，不拷贝，直接返回节点的指针
      std::vector<LTLNode*>
        get_level_order();
      
      // 判断这个LTL是不是一个只有布尔逻辑运算符的LTL
      bool is_boolean_formula() const;
  };
}

namespace ltl {
    std::string remove_blank(const std::string&);
    std::vector<std::string> split_into_in_order(const std::string& s);
    std::vector<std::string> in_order_to_post_order(const std::vector<std::string>& inorder);
}
