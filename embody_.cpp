#include "log_.h"
#include <sstream>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <ctime>
#include <chrono>
#include <format>   //C++20
#include <map>
#include <functional>

namespace sylar{
    void Logger::log(LogEvent::ptr event){
        // //如果想要查看的级别大于等于当前日志器能查看的级别，那么才进行输出
        if(event->getLevel() >= level_){
            for(auto& i : appender_)
                i->log(event);
        }
    }

    // level转换为字符串
    const char* LogLevel::ToString(LogLevel::Level level){
        switch(level){
        #define XX(name)         \
            case LogLevel::name: \
                return #name;
                break;
            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);
            XX(FATAL);

        #undef XX
            default:
                return "UNKNOW";
            }
            return "UNKNOW";
    }




    void FileLogAppender::log(LogEvent::ptr event){}

    // 添加删除迭代器
    void Logger::addAppender(LogAppender::ptr appender) { appender_.push_back(appender); }
    void Logger::delAppender(LogAppender::ptr appender) {
        for (auto it = appender_.begin(); it != appender_.end();++it){
            if(*it == appender){
                appender_.erase(it);
                break;
            }
        }
    }
    
    class MessageFormatItem : public LogFormatter::FormaterItem{
    public:
        MessageFormatItem(const std::string& str = ""){}
        void format(std::ostream &os, LogEvent::ptr event) override { os << event->getSS().str(); }
    };

    class LevelFormatItem : public LogFormatter::FormaterItem{
    public:
            LevelFormatItem(const std::string& str = ""){}
            void format(std::ostream& os, LogEvent::ptr event) override {
                os << LogLevel::ToString(event->getLevel());
            }
    };

    class ElapseFormatItem : public LogFormatter::FormaterItem{
    public:
        ElapseFormatItem(const std::string& str = ""){}
        void format(std::ostream& os, LogEvent::ptr event) override{
            os << event->getElapse();
        }
    };

    class NameFormatItem : public LogFormatter::FormaterItem{
    public:
        NameFormatItem(const std::string& str = ""){}
        void format(std::ostream& os, LogEvent::ptr event) override{
            os << event->getLogName();
        }
    };

    class ThreadIdFormatItem : public LogFormatter::FormaterItem{
    public:
        ThreadIdFormatItem(const std::string& str=""){}
        void format(std::ostream& os, LogEvent::ptr event) override{
            os << event->getThreadId();
        }
    };

    class FiberIdFormatItem: public LogFormatter::FormaterItem{
    public:
        FiberIdFormatItem(const std::string& str = ""){}
        void format(std::ostream& os, LogEvent::ptr event)override{
            os << event->getFiberId();
        }

    };

    class StringFormatItem : public LogFormatter::FormaterItem{
    public:
        StringFormatItem(const std::string& str) : string_(str){}
        void format(std::ostream& os, LogEvent::ptr event)override{
            os << string_;
        }
    private:
        std::string string_;
    };

    class DateTimeFormatItem : public LogFormatter::FormaterItem
    {
    public:
        DateTimeFormatItem(const std::string& format="%Y-%m-%d %H:%M:%S"):format_(format){}
        // 把系统时间的长数字转换为人可以认识的形式
        void format(std::ostream& os, LogEvent::ptr event) override{
            // 把时间戳转换成system_clock的时间点
            auto tp = std::chrono::system_clock::from_time_t(event->getTime());
            // 只要秒就行了，不要纳秒
            auto tp_sec = std::chrono::floor<std::chrono::seconds>(tp);
            // 转换成带时区的时间
            auto zt = std::chrono::zoned_time{std::chrono::current_zone(), tp_sec};
            std::string cpp20_fmt = "{:" + format_ + "}";
            try{
                std::string timeStr = std::vformat(cpp20_fmt, std::make_format_args(zt));
                os << timeStr;
            } catch(const std::exception& e){
                os << "TimeFormatError";
            }
        }
    private:
        std::string format_;
    };

    class FilenameFormatItem : public LogFormatter::FormaterItem{
    public:
        FilenameFormatItem(const std::string str = ""){}
        void format(std::ostream& os, LogEvent::ptr event){
            os << event->getFile();
        }
    };

    class LineFormatItem : public LogFormatter::FormaterItem
    {
    public:
        LineFormatItem(const std::string &str){}
        void format(std::ostream &os, LogEvent::ptr event) override
        {
            os << event->getLine();
        }
    };

    class NewLineFormatItem:public LogFormatter::FormaterItem{
    public:
        NewLineFormatItem(const std::string& str = ""){}
        void format(std::ostream& os, LogEvent::ptr event) override{
            os << std::endl;
        }
    };

    class TabFormatItem : public LogFormatter::FormaterItem
    {
    public:
        TabFormatItem(const std::string& str = ""){}
        void format(std::ostream& os, LogEvent::ptr event){
            os << "\t";
        }
    };


    // %d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n
    void LogFormatter::init(){
        // <格式字符, 附加参数(如时间格式), 类型>, 0: 普通字符串, 1: 格式化项
        std::vector<std::tuple<std::string, std::string, int>> vec;
        std::string nstr;
        for (auto i = 0; i < pattern_.size();i++){
            if(pattern_[i]!='%'){
                nstr.append(1, pattern_[i]);
                continue;
            }
            // 两个%当作一个普通的%。转义字符
            if((i+1)<pattern_.size()){
                if(pattern_[i+1]=='%'){
                    nstr.append(1, '%');
                    i++;
                    continue;
                }
            }
            // 此时的i应该指向的%
            auto n = i + 1;         // 指向%的后面一个字符
            auto fmt_status = 0;    // 1是{}内的内容，0不是
            auto fmt_begin = 0;     // { 开始的位置
            std::string str;
            std::string fmt;        // 存放'{}' 里面的字符
            while(n < pattern_.size())
            {
                if(!fmt_status and (!isalpha(pattern_[n]) and pattern_[n]!='{' and pattern_[n]!='}')){
                    str = pattern_.substr(i + 1, n - i - 1);
                    break;
                }
                if(fmt_status==0){
                    if(pattern_[n] =='{'){
                        str = pattern_.substr(i + 1, n - i - 1);
                        fmt_status = 1; //进入括号解析模式
                        fmt_begin = n;
                        n++;
                        continue;
                    }
                }else if (fmt_status==1){
                    if (pattern_[n] == '}'){
                        fmt = pattern_.substr(fmt_begin+1, n-1-fmt_begin);    
                        fmt_status = 0;
                        n++;
                        break;
                    }
                }
                n++;
                // 如果遍历完了
                if(n==pattern_.size()){
                    if(str.empty()){
                        str = pattern_.substr(i + 1);
                    }
                }
            }
            if(fmt_status==0){
                if(!nstr.empty()){
                    // 保存其他字符'[',']',':'
                    vec.push_back(std::make_tuple(nstr, std::string(),0));
                    nstr.clear();
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n - 1;
            }
            else if (fmt_status == 1)
            {
                std::cout << "pattern parse error: " << pattern_ << " - " << pattern_.substr(i) << std::endl;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
        }    // for end

        if(!nstr.empty()){
            vec.push_back(std::make_tuple(nstr, "", 0));
        }

        static std::map<std::string, std::function<LogFormatter::FormaterItem::ptr(const std::string& str)>> s_format_items = {
#define XX(str, C)\
            {#str, [](const std::string& fmt) {return LogFormatter::FormaterItem::ptr(new C(fmt));}}\

            XX(m, MessageFormatItem),
            XX(p, LevelFormatItem),
            XX(r, ElapseFormatItem),
            XX(c, NameFormatItem),
            XX(t, ThreadIdFormatItem),
            XX(n, NewLineFormatItem),
            XX(d, DateTimeFormatItem),
            XX(f, FilenameFormatItem),
            XX(l, LineFormatItem),
            XX(T, TabFormatItem),
            XX(F, FiberIdFormatItem),
#undef XX
        };

        // 组装 items_
        for(auto& i : vec){
            if(std::get<2>(i) == 0){
                items_.push_back(FormaterItem::ptr(new StringFormatItem(std::get<0>(i))));
            }else{
                auto it = s_format_items.find(std::get<0>(i));
                // 如果格式符错误就打印错误信息
                if(it == s_format_items.end()){
                    items_.push_back(FormaterItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                } else {
                    items_.push_back(it->second(std::get<1>(i)));
                }
            }
        }
    }

    std::string LogFormatter::format(LogEvent::ptr event){
        std::stringstream ss;
        for(auto i : items_){
            i->format(ss, event);
        }
        return ss.str();
    }

    void StdoutLogAppender::log(LogEvent::ptr event){
        std::cout << formatter_->format(event);
    }



} // namespace end

using namespace sylar;

int main(int argc,char** argv){
    Logger::ptr lg(new Logger("JYF"));
    //创建一个日志事件(这里的内容随便定义，因为我们没有真正用到它)
    LogEvent::ptr event(new LogEvent(
        lg->getName(),
        LogLevel::INFO,     //日志级别
        __FILE__,           //文件名称
        __LINE__,           //行号
        1234567,            //运行时间
        syscall(SYS_gettid),//线程ID
        0,                  //协程ID
        time(0)             //当前时间
    ));
    LogFormatter::ptr formatter(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    StdoutLogAppender::ptr stdApd(new StdoutLogAppender());
    stdApd->setFormatter(formatter);
    lg->addAppender(stdApd);
    event->getSS() << "this is a stringstream";
    lg->log(event);
    return 0;
}

