#include "logger/Logger.h"
#include "logger/LogEvent.h"

namespace sylar{
    class LogEventWrap{
    public:
         /**
         * @brief 构造函数
         * @param[in] e 日志事件
         */
        LogEventWrap(LogEvent::ptr e);
        
        /**
         * @brief 析构函数
         */
        ~LogEventWrap();

        /**
         * @brief 获取日志事件
         */
        LogEvent::ptr getEvent() const { return event_; }

        /**
         * @brief 获取日志内容流
         */
        std::stringstream &getSS();

    private:
        Logger::ptr logger_;
        LogEvent::ptr event_;
    };

} // sylar name 