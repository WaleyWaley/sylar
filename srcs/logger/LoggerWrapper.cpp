#include "logger/LoggerWrapper.h"

using namespace sylar;

LogEventWrap::LogEventWrap(LogEvent::ptr e) : event_(e) {}

LogEventWrap::~LogEventWrap(){
    event_->getLogger()->log(event_->getLevel(), event_);
}

std::stringstream &LogEventWrap::getSS() { return event_->getSS(); }