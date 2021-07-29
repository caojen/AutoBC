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

namespace autobc{
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

      class LTLNode;
      class PreNode;

      typedef LTL::LTLNode LTLNode;
      typedef LTL::PreNode PreNode;

      std::shared_ptr<LTLNode> root = nullptr;

      static LTL Gen(const std::string& s);
      std::string serialize() const;
      bool operator<(const LTL& other) const;

      // 深复制一个LTL，包括内部所有的LTLNode和PreNode都被深复制
      // 符号仍然直接浅复制
      LTL() = default;
      LTL(const LTL& other);

      class PreNode {
        public:
          std::shared_ptr<Op1>      op;
          std::shared_ptr<LTLNode>  to;         // 指向该操作数

          std::string serialize() const {
            std::string ret = op->str();
            ret += to->serialize();
            return ret;
          }

          PreNode() {}
          PreNode(const PreNode& pre) {
            this->op = pre.op;
            if(pre.to.get()) {
              this->to = std::make_shared<LTLNode>(*pre.to);
            }
          }

      };

      class LTLNode {
        public:
          std::shared_ptr<LTLNode>        left    = nullptr;          // 指向第一个操作数
          std::shared_ptr<Op2>            op      = nullptr;          // 指向运算符
          std::shared_ptr<LTLNode>        right   = nullptr;          // 指向第二个操作数

          std::shared_ptr<PreNode>        pre     = nullptr;          // 指向前缀符号
          std::shared_ptr<Literal>        li      = nullptr;          // 指向文字

          LTLNode() = default;

          // 深复制一个LTLNode
          LTLNode(const LTLNode& other) {
            if(other.left.get()) {
              this->left = std::make_shared<LTLNode>(*other.left);; 
            }

            this->op = other.op;
            if(other.right.get()) {
              this->right = std::make_shared<LTLNode>(*other.right);
            }

            if(other.pre.get()) {
              this->pre = std::make_shared<PreNode>(*other.pre);;
            }
            this->li = other.li;
          }

          LTLNode(std::shared_ptr<Literal> li) {
            this->li = li;
          }

          LTLNode(std::shared_ptr<PreNode> pre) {
            this->pre = pre;
          }

          // 判断这个LTLNode是不是一个文字（如果是，代表这是个终结符）
          inline bool is_literal() const {
            return this->li && *this->li != "";
          }

          // 判断这个LTL是否指向一个PreNode
          inline bool is_pre() const {
            return this->pre != nullptr;
          }

          std::string serialize() const {
            std::string ret = "(";
            if(this->is_literal()) {
              // 只是一个文字
              ret += this->li->serialize();
            } else if(this->is_pre()) {
              // 是一个PreNode
              ret += this->pre->serialize();
            } else {
              // 是一个 x 或 x op y 形式的LTL
              ret += this->left->serialize();  // x 需要确保不是nullptr
              if(this->op != nullptr) {
                ret += this->op->str();           // 在op存在的情况下
                ret += this->right->serialize();  // right需要确保不是nullptr
              }
            }

            ret += ")";
            return ret;
          }
      };
  
    private:
      static std::shared_ptr<LTLNode> GenPart(const std::vector<std::string>& s, const std::map<unsigned, unsigned>& map, unsigned begin, unsigned end);
  };
}
