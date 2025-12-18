#include "log.h"
#include <functional>

namespace sylar{

    // 把Level的enum转化为string
    const char *LogLevel::ToString(LogLevel::Level Level)
    {
        switch(Level){
    #define XX(name)         \
            case LogLevel::name: \
                return #name;    \
                break;
            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);
            XX(FATAL);
    #undef XX   //define结束
            default:
                return "UNKNOW";
            }
    }

    // 列表初始化
    class LogEvent::LogEvent(LogLevel::Level level, const char* file, int32_t line, uin32_t elpase, uint32_t thread_id, uint32_t fiber_id. uint64_t time):m_level(level),m_file(file),m_line(line),m_elapse(elapse),m_threadId(threadId),m_fiber(fiberId){}

    // 列表初始化的形式的构造函数
    Logger::Logger(const std::string& name) : m_name(name) {}
    
    void Logger::addAppender(LogAppender::ptr appender){
        m_appenders.push_back(appender);
    }

    void Logger::delAppender(LogAppender::ptr appender){
        for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it)
        {
            if(*it == appender){
                m_appenders.erase(it);
                break;
            }
        }
    }

    void Logger::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
        if(level >= m_level){
            for(auto& i : m_appenders){
                i->log(logger, level, event);
            }
        }

 
    }

    void Logger::debug(LogEvent::ptr event){
        log(LogLevel::DEBUG, event);
    }
    void Logger::info(LogEvent::ptr event){
        log(LogLevel::INFO, event);
    }
    void Logger::warn(LogEvent::ptr event){
        log(LogLevel::WARN, event);
    }
    void Logger::error(LogEvent::ptr event){ log(LogLevel::ERROR, event); }

    void Logger::fatal(LogEvent::ptr event){
        log(LogLevel::FATAL, event);
    }

    // 构造函数
    FileLogAppender::FileLogAppender(const std::string& filename) : m_filename(filename){}
    

    bool FileLogAppender::reopen(){
        if(m_filestream){
            m_filestream.close();
        }
        m_filestream.open(m_filename);
        // !!的作用是非0值转成1,0值还是0,转化成真正的bool值
        return !!m_filestream;
    }

    void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
        // 当这条日志的等级 >= 设定的最低等级时才记录。
         if(level >= m_level){
            // 右边的文件通过<<传入了m_filestream代表的文件里
             m_filestream << m_formatter->format(logger, level, event);
         }
    }

    void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
        if (level >= m_level)
            std::cout << m_formatter->format(logger, level, event);
    }

    //
    LogFormatter::LogFormatter(const std::string &pattern):m_pattern(pattern){}

    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
        std::stringstream ss;
        for(auto& i: m_items)
            i->format(ss, logger, level, event);
        return ss.str();
    }

    void LogFormatter::init(){
        /*
            %m 消息
            %p 日志级别
            %c 日志器名称
            %d 日期时间，后面可跟一对括号指定时间格式，比如%d{%Y-%m-%d %H:%M:%S}，这里的格式字符与C语言strftime一致
            %r 该日志器创建后的累计运行毫秒数
            %f 文件名
            %l 行号
            %t 线程id
            %F 协程id
            %N 线程名称
            %% 百分号
            %T 制表符
            %n 换行
        */

        // 临时存储解析出来的结构：<格式字符, 附加参数(如时间格式), 类型>
        // 类型 0: 普通字符串, 1: 格式化项
        // string format type
        std::vector < std::tuple<std::string, std::string, int >> vec;
        std::string nstr; // 暂存普通字符串的暂存区

        for (auto i = 0; i < m_pattern.size();++i){
            // 如果不是%说明是一个普通字符
            if(m_pattern[i] != '%'){
                // 暂存普通字符串
                nstr.append(1, m_pattern[i]);
                continue;
            }
        
            // 如果是%，且后面还是%，说明是转义，输出一个真实的%
            if((i+1) < m_pattern.size()){
                if(m_pattern[i+1] == '%'){
                    nstr.append(1, '%');
                    continue;
                }
            }

            // 遇到了真实的格式字符(%d.%m)。先把之前的普通字符串存下,然后再清除暂存区
            if(!nstr.empty()){
                vec.push_back(std::make_tuple(nstr, "", 0));    // 类型0: 普通串
                nstr.clear();
            }

            // 开始解释%及其后面的内容
            // fmt_status = 0 (解析名字)：正在读取 %d 中的 d。正在读取 %d 中的 d。
            // fmt_status = 1 (解析参数)：正在读取 %d{...} 中花括号里的内容。
            auto fmt_status = 0;
            auto n = i + 1;    
            std::string str;    // 格式化内容(d,m)
            std::string fmt;    // 括号内的参数(%Y-%m-%d)

            auto fmt_begin = 0;

            // string substr (size_t pos, size_t len) const; 这是语法
            while (n < m_pattern.size())
            {
                // 如果遇到不是字母也不是括号，说明这个格式符解析完成了(比如空格)
                if(!fmt_status and (!isalpha(m_pattern[n]) and m_pattern[n] != '{' and m_pattern[n] != '}')){
                    str = m_pattern.substr(i + 1, n - i - 1);
                    break;
                }
                // 如果是解释格式字符
                if(fmt_status==0){
                    if(m_pattern[n]=='{'){
                        str = m_pattern.substr(i + 1, n - i - 1); //抠出格式字符
                        fmt_status = 1; // 进入括号解析模式
                        fmt_begin = n;
                        ++n;
                        continue;
                    }
                }else if(fmt_status == 1){
                    if (m_pattern[n] == '}'){
                        // 括号结束
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        fmt_status = 0;
                        ++n;
                        break;
                    }
                }
                ++n;
                // 如果只有%d 而没有 {}, 且遍历到了末尾
                if(n == m_pattern.size()){
                    if(str.empty()){
                        str = m_pattern.substr(i + 1);
                    }
                }
            }
            if(fmt_status == 0){
                if(!nstr.empty()){
                    vec.push_back(std::make_tuple(str, fmt, 1));
                    i = n - 1;
                }else if(fmt_status == 1){
                    std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
                    vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
                }
            } 
        }// for end

        if(!nstr.empty()){
            vec.push_back(std::make_tuple(nstr, "", 0));
        }

        /*
            %m -- 消息体
            %p -- level
            %r -- 启动后的时间
            %c -- 日志名称
            %t -- 线程id
            %n -- 换行
            %d -- 时间
            %f -- 文件名
            %l -- 行号
        */
        
        // std::function 的标准语法是： std::function< 返回值类型 ( 参数列表 ) >
        static std::map<std::string, std::function<LogFormatter::FormatItem::ptr(const std::string &str)>> s_format_items = {
        // #是字符串化操作符，把宏的一个参数转换为带双引号的C风格字符串
#define XX(str, C) \
        {#str, [] (const std::string &fmt) { return FormatItem::ptr(new C(fmt)); }}
            XX(m, MessageFormatItem),
            XX(p, LevelFormatItem),
            XX(r, ElapseFormatItem),
            XX(c, NameFormatItem),
            XX(t, ThreadIdFormatItem),
            XX(n, NewLineFormatItem),
            XX(d, DateTimeFormatItem),
            XX(f, FilenameFormatItem),
            XX(l, LineFormatItem),
#undef XX
        };

        for(auto& i : vec){
            if(std::get<2>(i) == 0){
                m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
            }else{
                auto it = s_format_items.find(std::get<0>(i));
                if(it == s_format_items.end()){
                    m_items.push_back(FormatItem::ptr(new StringFormattItem("<<error_format %" + std::get<0>(i) + ">>")));
                } else {
                    m_items.push_back(it->second(std::get<1>(i)));
                }
            }

            std::cout << std::get<0>(i) << " - " << std::get<1>(i) << std::get<2>(i) << std::endl;
        }
    } // init end

    class MessageFormatItem : public LogFormatter::FormatItem{
    public:
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
            os << event->getContent();
        }
    };

    class LevelFormatItem : public LogFormatter::FormatItem{
    public:
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
            os << LogLevel::ToString(level);
        }
    };

    class ElapseFormatItem : public LogFormatter::FormatItem{
    public:
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
            os << event->getElapse();
        }
    };

    class NameFormatItem : public LogFormatter::FormatItem{
    public:
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
            os << event->getName();
        }
    };

    class ThreadIdFormatItem : public LogFormatter::FormatItem{
    public:
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
            os << event->getThreadId();
        }
    };

    class FiberIdFormatItem : public LogFormatter::FormatItem{
    public:
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
            os << event->getFiberId();
        }
    };
    
    class DateTimeFormatItem: public LogFormatter::FormatItem{
    public:
        DateTimeFormatItem(const std::string& format = "%Y:%m:%d %H:%M:%S"):m_format(format){}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
            os << event->getFiberId();
        }

    private:
        std::string m_format;
    };

    class FilenameFormatItem : public LogFormatter::FormatItem{
    public:
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
            os << event->getFile();
        }
    };

    class LineFormatItem : public LogFormatter::FormatItem{
    public:
        void format(std::ostream& os, Logger::ptr Logger, LogLevel::Level level, LogEvent::ptr event) override{
            os << event->getLine();
        }
    };



} // namespace sylar 