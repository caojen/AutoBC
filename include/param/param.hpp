#pragma once

// 一个Helper类，帮助实现主函数获取输入

#include <string>
#include <map>
#include <set>

namespace param {
  class Param {
    public:
      Param() = default;

      void set(
        const std::string& brief,         // 例如， -s (-h被禁用)
        const std::string& detail,        // 例如， --start （--help被禁用）
        const std::string& key,           // 如何查找这个关键字，例如: start
        bool after,                       // 这个符号后面是否需要额外添加字符，true时例如 -s 100, false时后面省略100
        bool required,                    // 判断这个符号是否为必须的,
        const std::string& description,   // 这个符号的帮助说明
        const std::string& def            // 这个字段的默认数值，如果after为true，那么按照这个默认值来填；如果after为false，那么将会丢弃这个def字段（即使给定），使用"false"字符串
      );

      void run(int argc, char** argv);    // 格式化

      std::string get(const std::string& key) const;
      std::string operator[](const std::string& key) const;
    private:
      std::set<std::string> keys;
      std::map<std::string, std::string> briefs;
      std::map<std::string, std::string> details;
      std::map<std::string, bool> requireds;
      std::map<std::string, bool> afters;
      std::map<std::string, std::string> descriptions;
      std::map<std::string, std::string> defs;

      std::map<std::string, std::string> values;
  };
}
