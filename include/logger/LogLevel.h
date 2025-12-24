#pragma once
#include <vector>
#include <string>

namespace sylar{
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

    /**
     * @brief 日志级别转字符串
     * @param[in] level 日志级别 
     * @return 字符串形式的日志级别
     */
        static const char *ToString(LogLevel::Level Level);
    /**
     * @brief 字符串转日志级别
     * @param[in] str 字符串 
     * @return 日志级别
     * @note 不区分大小写
     */
        static LogLevel::Level FromString(const std::string &str);
        
    };

} // sylar