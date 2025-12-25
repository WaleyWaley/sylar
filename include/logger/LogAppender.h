#pragma once
#include "LogEvent.h"
#include "LogFormatter.h"
#include <memory>
#include <iostream>
// 假设你有一个数据库连接池或客户端类，比如 MySQLClient
// #include "my_db_client.h"

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

            // 返回配置字符串
            virtual std::string toYamlString() override ;

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

        /**
        *   @brief 输出到数据库的appender
        */
    class SqlLogAppender:public LogAppender{
    public: 
        typedef std::shared_ptr<SqlLogAppender> ptr;

        // 构造函数传入数据库配置，或者在内部获取单例的DB连接
        SqlLogAppender(const std::string& connectionStr);

        ~SqlLogAppender(){}

        // 核心重写方法
        void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;

        virtual bool reopen();

        virtual std::string toYamlString() override;
    private:
        std::string connectionStr_; // 比如127.0.0.1::8080
        // 数据库连接对象
        // MySQLConnection::ptr sqlConn_;
        void simulate_db_execute(const std::string& sql);
    };
} // sylar