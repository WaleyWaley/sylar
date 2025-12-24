#pragma once
#include "LogEvent.h"
#include "LogFormatter.h"
#include <memory>
#include <iostream>

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
            StdoutLogAppender();
            // 重写父类方法
            void log(LogEvent::ptr event) override;

             /**
             * @brief 将日志输出目标的配置转成YAML String
             */
            // std::string toYamlString() override;
    };

    class FileLogAppender:public LogAppender{
        public:
            typedef std::shared_ptr<FileLogAppender> ptr;

        /**
         * @brief 构造函数
         * @param[in] file 日志文件路径
         */
            FileLogAppender(const std::string &filename);

            void log(LogEvent::ptr event) override;

            bool reopen();

        private:
            std::string filename_;
        };

} // sylar