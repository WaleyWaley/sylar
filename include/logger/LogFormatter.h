#pragma once
#include <memory>    // for std::shared_ptr
#include <string>    // for std::string
#include <vector>    // for std::vector
#include <iostream>  // for std::ostream (关键！)
#include <map>       // 后面 map 用到了
#include <functional> // functional 用到了
#include "logger/LogEvent.h"

namespace sylar{
    class LogFormatter{
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        LogFormatter(const std::string &pattern);
        void init();
        std::string format(LogEvent::ptr event);

        // 用来解析各种格式符号
    class FormaterItem{
    public:
        typedef std::shared_ptr<FormaterItem> ptr;
        // 有子类 要虚析构
        virtual ~FormaterItem(){};
        virtual void format(std::ostream &os, LogEvent::ptr event) = 0;
    };

    private:
        std::string pattern_;
        std::vector<LogFormatter::FormaterItem::ptr> items_;
    };

} // sylar