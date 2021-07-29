#pragma once

#include <iostream>
#include <deque>
#include <list>
#include <memory>
#include <string>
#include <map>
#include <stack>

#include "literal.hpp"
#include "operator.hpp"
#include "dict.hpp"

namespace ltl{
  class LTL;
  
  // LTL语句生成器
  class LTLGenerator {
    public:
      // 新建一个空的LTL公式
      LTLGenerator();
      // 新建一个只包含一个文字的LTL公式
      LTLGenerator(const Literal& li);
      // 拷贝构造函数
      LTLGenerator(const LTLGenerator& pro);
      
      // 执行运算

      // 一元算子应用于自身:
      // f1 & f2 ===op===> op(f1 & f2)
      LTLGenerator next() const;
      LTLGenerator finally() const;
      LTLGenerator global() const;
      LTLGenerator nnot() const;

      // 二元算子需要一个额外的操作数
      LTLGenerator until(const LTLGenerator& ltl) const;
      LTLGenerator release(const LTLGenerator& ltl) const;
      LTLGenerator aand(const LTLGenerator& ltl) const;
      LTLGenerator oor(const LTLGenerator& ltl) const;

      ~LTLGenerator();

      // 重写流
      friend std::ostream& operator<<(std::ostream& o, const LTLGenerator& ltl);
      std::string serialize() const;

      // 比较两个LTL公式是否完全一样
      bool operator==(const LTLGenerator& other) const;

      // 判断这个LTL是不是空的(没有任何文字)
      bool empty() const;

      // 判断这个LTL是不是只是一个文字
      bool singal() const;

      // 辅助构造函数
      static LTLGenerator Gen(const LTLGenerator&);

      // 最终生成LTL公式(将LTL生成器转换成LTL三叉树)
      LTL finalize() const;

      // 直接通过字符串来生成这个LTL
      static LTL parse(const std::string& s);

    private:
      // 该公式 “前缀” 一元运算符
      std::list<std::shared_ptr<Operator>> preOps;

      // 表明该公式只是一个简单的文字:
      Literal               li;
      
      // 如果该公式只有一部分，或者该公式有两部分:
      std::shared_ptr<LTLGenerator>  left;
      // 如果该公式只有一部分，那么中间的运算符号应该是EmptyOp
      // 否则，指明该二元运算符
      std::shared_ptr<Op2>  mid;
      // 如果mid不是EmptyOp，那么应该提供right
      std::shared_ptr<LTLGenerator>  right;
  };

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
          bool is_op1() const;
          bool is_op2() const;

          friend std::ostream& operator<<(std::ostream& o, const LTLNode& ltlNode);
          std::string serialize() const;

          // 构造函数
          LTLNode() {};
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
      };

      LTL() = default;

      // 深复制拷贝构造函数
      LTL(const LTL& ltl) {
        this->root = std::make_shared<LTLNode>(*ltl.root);
      }

      // 移动构造函数
      LTL(LTL&& ltl) {
        this->root = ltl.root;
      }

      std::shared_ptr<LTLNode> root;

      friend std::ostream& operator<<(std::ostream& o, const LTL& ltl);
      std::string serialize() const;

      // 根据字符串来返回对应的LTL公式
      // 错误时抛出not_a_ltl错误
      static LTL parse(const std::string& s);

      bool operator<(const LTL& other) const;
  };
}
