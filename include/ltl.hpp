#pragma once

#include <iostream>
#include <deque>
#include <list>
#include <memory>

#include "literal.hpp"
#include "operator.hpp"

namespace autobc{
  // 表示一个LTL语句
  class LTL {
    public:
      // 新建一个空的LTL公式
      LTL();
      // 新建一个只包含一个文字的LTL公式
      LTL(const Literal& li);
      // 拷贝构造函数
      LTL(const LTL& pro);
      
      // 执行运算

      // 一元算子应用于自身:
      // f1 & f2 ===op===> op(f1 & f2)
      LTL next() const;
      LTL finally() const;
      LTL global() const;
      LTL nnot() const;

      // 二元算子需要一个额外的操作数
      LTL until(const LTL& ltl) const;
      LTL release(const LTL& ltl) const;
      LTL aand(const LTL& ltl) const;
      LTL oor(const LTL& ltl) const;

      ~LTL();

      // 重写流
      friend std::ostream& operator<<(std::ostream& o, const LTL& ltl);
      std::string serialize() const;

      // 比较两个LTL公式是否完全一样
      bool operator==(const LTL& other) const;

      // 判断这个LTL是不是空的(没有任何文字)
      bool empty() const;

      // 判断这个LTL是不是只是一个文字
      bool singal() const;

      // 辅助构造函数
      static LTL Gen(const LTL&);

    private:
      // 该公式 “前缀” 一元运算符
      std::list<std::shared_ptr<Operator>> preOps;

      // 表明该公式只是一个简单的文字:
      Literal               li;
      
      // 如果该公式只有一部分，或者该公式有两部分:
      std::shared_ptr<LTL>  left;
      // 如果该公式只有一部分，那么中间的运算符号应该是EmptyOp
      // 否则，指明该二元运算符
      std::shared_ptr<Op2>  mid;
      // 如果mid不是EmptyOp，那么应该提供right
      std::shared_ptr<LTL>  right;
  };
}
