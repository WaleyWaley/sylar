#pragma once
#include "LogLevel.h"
#include <memory>
#include <cstdint> // for uint32_t uint64_t
#include <string>
#include <sstream>

namespace sylar{
    class LogEvent {
    public:
        typedef std::shared_ptr<LogEvent> ptr;
     /**
     * @brief 构造函数
     * @param[in] logger 日志器
     * @param[in] level 日志级别
     * @param[in] file 文件名
     * @param[in] line 文件行号
     * @param[in] elapse 程序启动依赖的耗时(毫秒)
     * @param[in] threadId 线程id
     * @param[in] fiberId 协程id
     * @param[in] time 日志事件(秒)
     * @param[in] thread_name 线程名称
     */
        LogEvent(std::string logName,LogLevel::Level level, const char *file, int32_t line, uint32_t elapse, uint32_t threadId, uint32_t fiberId, uint64_t time);

        const char* getFile() const { return file_;}
        int32_t getLine() const { return line_;}
        uint32_t getElapse() const { return elapse_;}
        uint32_t getThreadId() const { return threadId_;}
        uint32_t getFiberId() const { return fiberId_;}
        uint64_t getTime() const { return time_;}
        LogLevel::Level getLevel() const { return level_;}
        std::string getLogName() const { return logName_;}
        std::stringstream& getSS() { return ss_; }

    private:
        LogLevel::Level level_;       //日志级别           
        const char* file_ = nullptr;  //文件名
        int32_t line_ = 0;            //行号
        uint32_t elapse_ = 0;         //程序启动开始到现在的毫秒数
        uint32_t threadId_ = 0;       //线程id
        uint32_t fiberId_ = 0;        //协程id
        uint64_t time_ = 0;           //时间戳
        std::string logName_;         //日志名
        std::stringstream ss_;       //字符流
    };



} // sylar