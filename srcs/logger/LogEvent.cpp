#include <unistd.h>
#include "logger/LogLevel.h"
#include "logger/LogEvent.h"
// #include "logger/LogAppender.h"
using namespace sylar;

LogEvent::LogEvent(std::string logName, LogLevel::Level level, const char *file, int32_t line, uint32_t elapse, uint32_t threadId, uint32_t fiberId, uint64_t time) : logName_(logName),level_(level), file_(file), line_(line), elapse_(elapse), threadId_(threadId), fiberId_(fiberId), time_(time) {}