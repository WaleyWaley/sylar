#include "log_.h"
#include <ctime>
#include <chrono>
#include <format>   //C++20

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


    void StdoutLogAppender::log(LogEvent::ptr event){
        // // 把时间戳转换成 system_clock 的时间点
        // auto tp = std::chrono::system_clock::from_time_t(event->getTime());
        // // 把精度缩小到秒, floor 意思是“向下取整”，这里特指把时间精度取整到 seconds
        // auto tp_sec = std::chrono::floor<std::chrono::seconds>(tp);
        // // 转换成带时区的时间
        // auto zt = std::chrono::zoned_time{std::chrono::current_zone(), tp_sec};
        // // 直接格式化
        // std::string timeStr = std::format("{:%Y-%m-%d %H:%M:%S}", zt);
        
        // std::cout
        //     << timeStr << "    "
        //     << event->getThreadId() << "    "
        //     << event->getFiberId() << "    "
        //     << "["
        //     << LogLevel::ToString(event->getLevel())
        //     << "]    " 
        //     << event->getFile() << ":" << event->getLine() << "    "
        //     << "输出到控制台的信息"
        //     << std::endl;
        
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
    
    // %d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n
    void LogFormatter::init(){
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
                    vec.push_back(std::make_tuple(nstr, std::string(), 0));
                    nstr.clear();
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n - 1;
            }else if(fmt_status==1){
                std::cout << "pattern parse error: " << pattern_ << " - " << pattern_.substr(i) << std::endl;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
        }
        if(!nstr.empty()){
            vec.push_back(std::make_tuple(nstr, "", 0));
        }

        for(auto& it : vec){
            std::cout
                << std::get<0>(it)
                << " : " << std::get<1>(it)
                << " : " << std::get<2>(it)
                << std::endl;
        }
    }


    class MessageFormatItem : public LogFormatter::FormaterItem{
    public:
        MessageFormatItem(const std::string& str = ""){}
        void format(std::ostream &os, LogEvent::ptr event) override { os << "Message"; }
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
            os << "Name";
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
            std::string timeStr = std::vformat(format_, std::make_format_args(zt));
            os << timeStr;
        }
    private:
        std::string format_;
    };

    class LineFormatItem : public LogFormatter::FormaterItem{
    public: 
        LineFormatItem(const std::string& str) : string_(str){}
        void format(std::ostream& os, LogEvent::ptr event) override{
            os << string_;
        }
    private:
        std::string string_;
    };

    class TabFormatItem : public LogFormatter::FormaterItem{
    public:
        TabFormatItem(const std::string& str = ""){}
        void format(std::ostream& os, LogEvent::ptr event){
            os << "\t";
        }
    };

} // namespace end

using namespace sylar;

int main(int argc, char** argv){
   //创建一个日志事件(这里的内容随便定义，因为我们没有真正用到它)
    LogEvent::ptr event(new LogEvent(
        LogLevel::INFO,
        "log.txt", 
        1, 
        1234567, 
        2,
        3, 
        time(0)
    ));
    LogFormatter::ptr formatter(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    formatter->format(event);
    return 0;
}
