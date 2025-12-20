#include "logger/LoggerWrapper.h"

using namespace sylar;

LogEventWrap::LogEventWrap(Logger::ptr logger, LogEvent::ptr e) : logger_(logger), event_(e) {}

LogEventWrap::~LogEventWrap(){
    logger_->log(event_);
}

std::stringstream &LogEventWrap::getSS() { return event_->getSS(); }