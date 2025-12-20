#pragma once
#include <vector>

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
            // 把enum变量转换为字符串比如:Level::DEBUG -> "DEBUG"
        static const char *ToString(LogLevel::Level Level);
    };

} // sylar