#include <iostream>
#include <string>   //日志名称用到了字符串
#include <memory>   //智能指针所需的头文件
#include <stdint.h>
#include <list>
#include <vector>
#include <sstream>

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
    
    class LogFormatter{
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        LogFormatter(const std::string &pattern);
        void init();
        std::string format(LogEvent::ptr event);

        // 用来解析各种格式符号
        class FormaterItem{
        public:
            typedef std::shared_ptr<FormaterItem> ptr;
            // 有子类 要虚析构
            virtual ~FormaterItem(){};
            virtual void format(std::ostream &os, LogEvent::ptr event) = 0;
        };

    private:
        std::string pattern_;
        std::vector<LogFormatter::FormaterItem::ptr> items_;
    };
    class LogEvent {
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent(std::string logName,LogLevel::Level level, const char *file, int32_t line, uint32_t elapse, uint32_t threadId, uint32_t fiberId, uint64_t time);

        const char* getFile() const { return file_;}
        int32_t getLine() const { return line_;}
        uint32_t getElapse() const { return elapse_;}
        uint32_t getThreadId() const { return threadId_;}
        uint32_t getFiberId() const { return fiberId_;}
        uint64_t getTime() const { return time_;}
        LogLevel::Level getLevel() const { return level_;}
        std::string getLogName() const { return logName_;}
        std::stringstream& getSS() { return ss_; }

    private:
        LogLevel::Level level_;       //日志级别           
        const char* file_ = nullptr;  //文件名
        int32_t line_ = 0;            //行号
        uint32_t elapse_ = 0;         //程序启动开始到现在的毫秒数
        uint32_t threadId_ = 0;       //线程id
        uint32_t fiberId_ = 0;        //协程id
        uint64_t time_ = 0;           //时间戳
        std::string logName_;         //日志名
        std::stringstream ss_;       //字符流
    };
    LogEvent::LogEvent(std::string logName, LogLevel::Level level, const char *file, int32_t line, uint32_t elapse, uint32_t threadId, uint32_t fiberId, uint64_t time) : logName_(logName),level_(level), file_(file), line_(line), elapse_(elapse), threadId_(threadId), fiberId_(fiberId), time_(time) {}

    class LogFormatter{
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        LogFormatter(const std::string &pattern);
        void init();
        std::string format(LogEvent::ptr event);

        // 用来解析各种格式符号
        class FormaterItem{
        public:
            typedef std::shared_ptr<FormaterItem> ptr;
            // 有子类 要虚析构
            virtual ~FormaterItem(){};
            virtual void format(std::ostream &os, LogEvent::ptr event) = 0;
        };

    private:
        std::string pattern_;
        std::vector<LogFormatter::FormaterItem::ptr> items_;
    };

    class LogAppender
    {
    public:
        // 定义智能指针
        typedef std::shared_ptr<LogAppender> ptr;
        // 虚析构 空函数没有复杂逻辑所以 直接定义掉
        virtual ~LogAppender() {}
        // 输出函数为纯虚函数，因为具体实现各个子类不一样，由各个子类自己决定
        virtual void log(LogEvent::ptr event) = 0;

        void setFormatter(LogFormatter::ptr val) { formatter_ = val; }
        LogFormatter::ptr getFormatter() const { return formatter_; }

    protected:
        LogFormatter::ptr formatter_;
    };

    class Logger{
    public:
        typedef std ::shared_ptr<Logger> ptr;

        //日志名称有且仅有构造时可以指定，如果未指定则给个默认名称 root 
        //这里传引用类型是为了避免不必要的拷贝操作
        //使用const是为了规定这个名称在内部不再会被改变
        Logger(const std::string &name = "root");

        //希望名称能被获取 但不应该被改变所以用const 为避免无用的拷贝所以返回引用类型
        const std::string &getName() const { return name_; }
        LogLevel::Level getLevel() const { return level_; }
        void setLevel(LogLevel::Level val) { level_ = val; }

        //定义了一个输出日志的方法 传入想要查看的最大日志级别
        void log(LogEvent::ptr event);
        // 新增一个适配器
        void addAppender(LogAppender::ptr appender);
        // 删除一个适配器
        void delAppender(LogAppender::ptr appender);
    private:
        std::string name_;
        LogLevel::Level level_;
        // Appender
        std::list<LogAppender::ptr> appender_;
    };
    Logger::Logger(const std::string &name) : name_(name) { level_ = LogLevel::DEBUG; }


    // 在初始化的时候就把pattern解析好
    LogFormatter::LogFormatter(const std::string& pattern):pattern_(pattern){init();}


    class StdoutLogAppender:public LogAppender{
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        StdoutLogAppender(){
            formatter_.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
        }
        // 重写父类方法
        void log(LogEvent::ptr event) override;
    };

    
    class FileLogAppender:public LogAppender{
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
        FileLogAppender(const std::string &filename);
        void log(LogEvent::ptr event) override;
    private:
        std::string filename_;
    };
    FileLogAppender::FileLogAppender(const std::string& filename) : filename_(filename){}








} // namespace end