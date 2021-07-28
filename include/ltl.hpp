#pragma once

#include <iostream>
#include <list>
#include <vector>
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
      virtual LTL next() const;
      virtual LTL finally() const;
      virtual LTL global() const;
      virtual LTL nnot() const;

      // 二元算子需要一个额外的操作数
      virtual LTL until(const LTL& ltl) const;
      virtual LTL release(const LTL& ltl) const;
      virtual LTL aand(const LTL& ltl) const;
      virtual LTL oor(const LTL& ltl) const;

      virtual ~LTL();

      // 重写流
      friend std::ostream& operator<<(std::ostream& o, const LTL& ltl);

      // 比较两个LTL公式是否完全一样
      bool operator==(const LTL& other) const;

    private:
      // 该公式 “前缀” 一元运算符
      std::vector<Operator> preOps;
      // 如果该公式只有一部分，或者该公式有两部分:
      std::shared_ptr<LTL>  left;
      // 如果该公式只有一部分，那么中间的运算符号应该是EmptyOp
      // 否则，指明该二元运算符
      std::shared_ptr<Op2>  mid;
      // 如果mid不是EmptyOp，那么应该提供right
      std::shared_ptr<LTL>  right;
  };
}
