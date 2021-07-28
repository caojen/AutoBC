#pragma once

#include <iostream>
#include <deque>
#include <list>
#include <memory>
#include <string>

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

      std::shared_ptr<LTLNode> ltlroot;
      std::shared_ptr<PreNode> preroot;

      static LTL Gen(const std::string& s);

      class PreNode {
        public:
          std::list<std::shared_ptr<Op1>> ops;        // 顺序存放所有前缀符号
          std::shared_ptr<LTLNode>        to;         // 指向该操作数

          std::string serialize() const {
            std::string ret = "";
            for(auto& op: this->ops) {
              ret += op->str();
            }
            ret += to->serialize();
            return ret;
          }

      };

      class LTLNode {
        public:
          std::shared_ptr<LTLNode>        left    = nullptr;          // 指向第一个操作数
          std::shared_ptr<Op2>            op      = nullptr;          // 指向运算符
          std::shared_ptr<LTLNode>        right   = nullptr;          // 指向第二个操作数

          std::shared_ptr<PreNode>        pre     = nullptr;          // 指向前缀符号
          std::shared_ptr<Literal>        li      = nullptr;          // 指向文字

          LTLNode(std::shared_ptr<Literal> li) {
            this->li = li;
          }

          LTLNode(std::shared_ptr<PreNode> pre) {
            this->pre = pre;
          }

          // 判断这个LTLNode是不是一个文字（如果是，代表这是个终结符）
          inline bool is_literal() const {
            return *this->li == "";
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
      static LTL GenPart(const std::vector<std::string>& s, unsigned begin, unsigned end);
  };
}
