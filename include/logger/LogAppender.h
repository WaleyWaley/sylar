#pragma once
#include "LogEvent.h"
#include "LogFormatter.h"
#include <memory>

namespace sylar{
    class LogAppender
        {
        public:
            // 定义智能指针
            typedef std::shared_ptr<LogAppender> ptr;
            // 虚析构 空函数没有复杂逻辑所以 直接定义掉
            virtual ~LogAppender() {}
            // 输出函数为纯虚函数，因为具体实现各个子类不一样，由各个子类自己决定
            virtual void log(LogEvent::ptr event) = 0;

            void setFormatter(LogFormatter::ptr val) { formatter_ = val; }
            LogFormatter::ptr getFormatter() const { return formatter_; }

        protected:
            LogFormatter::ptr formatter_;
        };

    class StdoutLogAppender:public LogAppender{
        public:
            typedef std::shared_ptr<StdoutLogAppender> ptr;
            StdoutLogAppender(){
                formatter_.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
            }
            // 重写父类方法
            void log(LogEvent::ptr event) override;
        };

    class FileLogAppender:public LogAppender{
        public:
            typedef std::shared_ptr<FileLogAppender> ptr;
            FileLogAppender(const std::string &filename);
            void log(LogEvent::ptr event) override;
        private:
            std::string filename_;
        };

} // sylar