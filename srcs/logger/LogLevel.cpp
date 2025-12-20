#include "logger/LogLevel.h"

using namespace sylar;

// level转换为字符串
const char* LogLevel::ToString(LogLevel::Level level){
    switch(level){
    #define XX(name)         \
        case LogLevel::name: \
            return #name;
            break;
        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAL);

    #undef XX
        default:
            return "UNKNOW";
        }
        return "UNKNOW";
}