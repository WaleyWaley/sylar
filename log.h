#pragma once

#include<string>
#include<stdint.h>
#include<memory>
#include<list>
#include<sstream>   // std::stringstream
#include<fstream>
#include<vector>
#include <iostream>
#include <map>


namespace sylar{
    // 日志事件
    class LogEvent{
        public:
            typedef std::shared_ptr<LogEvent> ptr;
            LogEvent();

            const char* getFile() const { return m_file; }
            int32_t getLine() const { return m_line; }
            uint32_t getElapse() const { return m_elapse; }
            uint32_t getThreadId() const { return m_threadId; }
            uint32_t getFiberId() const { return m_fiberId; }
            uint64_t getTime() const { return m_time; }
            const std::string &getContent() const { return m_content; }
            // Loglevel::Level getLevel() const { return m_level; };

        private:
            const char* m_file = nullptr;   // 文件名
            int32_t m_line = 0;             // 行号
            uint32_t m_elapse = 0;          // 程序启动开始到现在的毫秒数
            uint32_t m_threadId = 0;        // 线程id
            uint32_t m_fiberId = 0;         // 协程id
            uint64_t m_time = 0;            // 时间戳

            std::string m_content;          // 消息体？
    };

    // 日志级别
    class LogLevel{
        public:
            enum Level
            {
                UNKNOW = 0,
                DEBUG = 1,
                INFO = 2,
                WARN = 3,
                ERROR = 4,
                FATAL = 5
            };
            // 把enum变量转换为字符串比如:Level::DEBUG -> "DEBUG"
            static const char *ToString(LogLevel::Level Level);
            LogLevel::Level getLevel();
    };

    // 日志格式器
    class LogFormatter{
        public:
            typedef std::shared_ptr<LogFormatter> ptr;
            // Constructor
            LogFormatter(const std::string &pattern);
            // &t    %thread_id %m%n
            std::string format(LogEvent::ptr event);

        private:
            class FormatItem{
                public:
                    typedef std::shared_ptr<FormatItem> ptr;
                    // 虚析构函数
                    virtual ~FormatItem(){};
                    // 纯虚函数，如果子类没有override，就只是一个声明
                    // 输出到文件
                    // 1. 使用 std::ostream& 作为父类，兼容 stringstream 和 ofstream
                    // 2. 必须加 & (引用)，流不能拷贝
                    virtual void format(LogLevel::Level level, LogEvent::ptr event) = 0;
            };
            void init();

        private:
            std::string m_pattern;
            std::vector<FormatItem::ptr> m_items;
    };

    // 日志输出地
    class LogAppender{
        public:
            typedef std::shared_ptr<LogAppender> ptr;
            // 虚函数需要子类自定义
            virtual ~LogAppender() {};
            virtual void log(LogLevel::Level level, LogEvent::ptr event);
            LogFormatter::ptr getFormatter() const{
                return m_formatter;
            }

        protected:
            LogLevel::Level m_level;
            LogFormatter::ptr m_formatter;


    };

    // 日志器
    class Logger{
        public:
            typedef std::shared_ptr<Logger> ptr;
            Logger(const std::string &name = "root");
            void log(LogLevel::Level level, LogEvent::ptr event);

            void debug(LogEvent::ptr event);
            void info(LogEvent::ptr event);
            void warn(LogEvent::ptr event);
            void error(LogEvent::ptr event);
            void fatal(LogEvent::ptr event);

            void addAppender(LogAppender::ptr appender);
            void delAppender(LogAppender::ptr appender);
            LogLevel::Level getLevel() const { return m_level; };
            void setLevel(LogLevel::Level val) { m_level = val; };

        private:
            std::string m_name;                     // 日志名称
            LogLevel::Level m_level;                // 日志级别
            std::list<LogAppender::ptr> m_appenders;  // Appender集合
    };

    // 输出到控制台的Appender
    class StdoutLogAppender : public LogAppender{
        public:
            typedef std::shared_ptr<StdoutLogAppender> ptr;
            void log(LogLevel::Level level, LogEvent::ptr event) override;
    };

    // 输出到文件的Appender 
    class FileLogAppender : public LogAppender{
        public:
            typedef std::shared_ptr<FileLogAppender> ptr;
            FileLogAppender(const std::string &filename);
            void log(LogLevel::Level level, LogEvent::ptr event) override;
            // 文件打开成功返回true
            bool reopen();

        protected:
            std::string m_filename;
            std::ofstream m_filestream;
    };
}