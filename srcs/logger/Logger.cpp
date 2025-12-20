#include "logger/Logger.h"

using namespace sylar;
// 设定一个初始的门槛等级DEBUG
Logger::Logger(const std::string &name) : name_(name) { level_ = LogLevel::DEBUG; }

void Logger::log(LogEvent::ptr event){
//如果想要查看的级别大于等于当前日志器能查看的级别，那么才进行输出
    if(event->getLevel() >= level_){
        for(auto& i : appender_)
            i->log(event);
    }
}
void Logger::addAppender(LogAppender::ptr appender)
{
    appender_.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender){
    for (auto it = appender_.begin(); it != appender_.end();it++)
    {
        if(*it == appender){
            appender_.erase(it);
            break;
        }
    }
}
