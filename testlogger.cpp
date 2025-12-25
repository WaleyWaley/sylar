#include <iostream>
#include "logger/Logger.h"
#include "logger/LogEvent.h"
#include "logger/LogFormatter.h"
#include "logger/LoggerWrapper.h"
#include "utils/utils.h"
// #include <syscall.h>
using namespace sylar;



int main(int argc, char **argv)
{
    std::cout << "======START======" << std::endl;
    Logger::ptr lg(new Logger("XYZ"));
    LogEvent::ptr event(new LogEvent(lg->getName(),       // 日志名称
                                     LogLevel::INFO,      // 日志级别
                                     __FILE__,            // 文件名称
                                     __LINE__,            // 行号
                                     12423,               // 运行时间
                                     syscall(SYS_gettid), // 线程ID
                                     0,                   // 协程ID
                                     time(0)              // 当前时间
                                     ));

    LogFormatter::ptr formatter(new LogFormatter(
        "%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));

    // 添加控制台输出适配器
    SqlLogAppender::ptr stdApd(new SqlLogAppender());

    fileApd->setFormatter(formatter);

    lg->addAppender(fileApd);

    // 使用宏
    SYLAR_LOG_FMT_LEVEL(lg, LogLevel::INFO, "Hello JYF!");

    std::cout << "=======END=======" << std::endl;
    return 0;
}
