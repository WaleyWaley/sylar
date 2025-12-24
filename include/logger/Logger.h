#pragma once
#include <memory>
#include <string>
#include <list>
#include "LogLevel.h"
#include "LogAppender.h"

namespace sylar{
    class Logger{
    public:
        typedef std ::shared_ptr<Logger> ptr;

        //日志名称有且仅有构造时可以指定，如果未指定则给个默认名称 root 
        //这里传引用类型是为了避免不必要的拷贝操作
        //使用const是为了规定这个名称在内部不再会被改变
        Logger(const std::string &name = "root");

        //希望名称能被获取 但不应该被改变所以用const 为避免无用的拷贝所以返回引用类型
        const std::string &getName() const { return name_; }
        LogLevel::Level getLevel() const { return level_; }
        void setLevel(LogLevel::Level val) { level_ = val; }

        //定义了一个输出日志的方法 传入想要查看的最大日志级别
        void log(LogEvent::ptr event);
        // 新增一个适配器
        void addAppender(LogAppender::ptr appender);
        // 删除一个适配器
        void delAppender(LogAppender::ptr appender);
        /**
        * @brief 将日志器的配置转成YAML String
        */
        std::string toYamlString();
    private:
        std::string name_;
        LogLevel::Level level_;
        // Appender
        std::list<LogAppender::ptr> appender_;

    };
} // sylar