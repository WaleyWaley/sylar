#include "logger/Logger.h"
#include "logger/LogEvent.h"

namespace sylar{
    /**
    *   @brief 日志事件包装器。将logEvent打包，可以直接通过使用该类完成对日志的定义。
    */
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