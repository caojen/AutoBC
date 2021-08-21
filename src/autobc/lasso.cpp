#include <queue>

#include "lasso.hpp"

using namespace ltl;

typedef LTL::LTLNode LTLNode;

namespace autobc {
  Lasso::Lasso(const LTL& ltl) {
    this->ltl = ltl;
    // Get Lasso terms of this->ltl, insert into this->terms and this->vec_terms
    auto in_order = split_into_in_order(this->ltl.serialize());

    auto post_order = in_order_to_post_order(in_order);
    
    std::stack<std::shared_ptr<LTL::LTLNode>> stack;

    // ret = <term, root>
    std::vector<std::pair<std::shared_ptr<LTL::LTLNode>, std::shared_ptr<LTL::LTLNode>>> tmp;

    for(auto& s: post_order) {
      auto op = Operator::gen(s);
      if(op == op::emptyOp) {
        stack.push(std::make_shared<LTL::LTLNode>(ltl::Literal(s)));
      } else {
        // 判断op是什么符号
        if(dynamic_cast<Op2*>(op.get()) != nullptr) {
          // 二元运算符时，无条件加入
          if(stack.size() < 2) {
            throw not_a_ltl();
          }
          std::shared_ptr<LTL::LTLNode> a = stack.top();
          stack.pop();
          std::shared_ptr<LTL::LTLNode> b = stack.top();
          stack.pop();
          if(!use_op_imply && dynamic_cast<Imply*>(op.get()) != nullptr) {
            auto not_b = std::make_shared<LTL::LTLNode>(op::nnot, b);
            stack.push(std::make_shared<LTL::LTLNode>(not_b, op::oor, a));
          } else {
            stack.push(std::make_shared<LTL::LTLNode>(b, op, a));
          }
        } else if(dynamic_cast<Op1*>(op.get()) != nullptr) {
          // 一元运算符时，如果是X或G，那么需要单独执行
          // 但无论如何都是要继续构造

          if(stack.empty()) {
            throw not_a_ltl();
          }

          auto top = stack.top();

          std::shared_ptr<LTL::LTLNode> a = stack.top();
          stack.pop();
          stack.push(std::make_shared<LTL::LTLNode>(op, a));

          if(op == op::next || op == op::finally) {
            // 如果是next 或 finally
            
            // 判断顶部是不是文字
            if(top->is_literal() || top->is_literal_negative()) {
              auto term = top;
              auto root = stack.top();
              tmp.push_back({term, root});
            } else {
              // 如果不是文字，那么判断是不是&
              if(top->op == op::aand) {
                auto left = top->left;
                auto right = top->right;
                auto leftis = (left->is_literal() || left->is_literal_negative() || left->op == op::aand) && (left->serialize().find('G') == std::string::npos);
                auto rightis = (right->is_literal() || right->is_literal_negative() || right->op == op::aand) && (right->serialize().find('G') == std::string::npos);
                auto root = stack.top();
                std::shared_ptr<LTL::LTLNode> term;
                if(leftis && !rightis) {
                  term = left;
                } else if(!leftis && rightis) {
                  term = right;
                } else if(leftis && rightis) {
                  term = top;
                }
                if(term) {
                  tmp.push_back({term, root});
                }
              }
            }

          } else if(op == op::global) {
            auto root = top;
            for(auto iter = tmp.begin(); iter != tmp.end(); ++iter) {
              if(iter->second == root) {
                tmp.erase(iter);
                break;
              }
            }
          }
        }
      }
    }
    for(auto& item: tmp) {
      auto insert_result = this->terms.insert(item.first);
      if(insert_result.second == true) {
        this->vec_terms.push_back(item.first);
      }
    }
  }

  std::set<ltl::LTL> Lasso::fetch_terms(unsigned int i) {
    if(i == 0 || i > this->terms.size()) {
      return {};
    }

    if(this->cache.find(i) != this->cache.end()) {
      return this->cache[i];
    } else {
      // 构造C(i, n)
      auto n = this->terms.size();
      C c(i, n);
      // 返回结果
      std::set<ltl::LTL> ret;

      auto list = c.next();
      while(!list.empty()) {
        ltl::LTL tmp;
        bool inited = false;
        for(auto& idx: list) {
          if(inited == false) {
            inited = true;
            tmp = this->vec_terms[idx - 1];
          } else {
            tmp = tmp.aand(this->vec_terms[idx - 1]);
          }
        }
        ret.insert(tmp);
      }
      this->cache[i] = ret;
      return ret;
    }
  }
}
