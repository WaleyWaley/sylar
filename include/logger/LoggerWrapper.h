#include "logger/Logger.h"
#include "logger/LogEvent.h"

namespace sylar{
    class LogEventWrap{
    public:
        LogEventWrap(Logger::ptr logger, LogEvent::ptr e);
        ~LogEventWrap();
        LogEvent::ptr getEvent() const { return event_; }
        std::stringstream &getSS();

    private:
        Logger::ptr logger_;
        LogEvent::ptr event_;
    };

} // sylar name 