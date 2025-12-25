#include "logger/LogAppender.h"
#include <iostream>
#include <fstream>


namespace sylar{
    FileLogAppender::FileLogAppender(const std::string& filename) : filename_(filename){}

    void FileLogAppender::log(LogEvent::ptr event) {
        std::ofstream fdopen;
        fdopen.open("log.txt", std::ios::app);
        if (fdopen)
        {
            fdopen << formatter_->format(event);
        } else{
            std::cout << "log.txt 打开失败" << std::endl;
        }
        fdopen.close();
    }

    std::string FileLogAppender::toYamlString() override{
        return "type: FileLogAppender";
    }

    StdoutLogAppender::StdoutLogAppender(){}
    void StdoutLogAppender::log(LogEvent::ptr event){
        std::cout << formatter_->format(event);
    }

    std::string StdoutLogAppender::toYamlString() override{
        return "type: StdoutLogAppender";
    }

    // 1. 构造函数实现
    // 使用初始化列表初始化成员变量 connectionStr_
    SqlLogAppender::SqlLogAppender(const std::string& connectionStr) : connectionStr_(connectionStr){
        // 初始化数据库连接
        // sqlConn_ = DBManager::GetInstance()->getConnection(connectionStr);
        std::cout << "[Init] 连接数据库: "  << connectionStr << " (模拟成功)" << std::endl;
    }

    std::string SqlLogAppende::toYamlString() override{
        return "type: SqlLogAppender";
    }

    void SqlLogAppender::log(std::Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event){
        if(level < level_)
            return;
        auto tp = std::chrono::system_clock::from_time_t(event->getTime());
        auto tp_sec = std::chrono::floor<std::chrono::seconds>(tp);
        // 先设置一个DB中常见的格式化形式
        std::string FormatStr = "{%Y-%m-%d %H:%M:%S}";
        std::string timeStr = std::vformat(FormatStr, std::make_format_args(tp_sec));

        // 4. 拼装 SQL 语句
        // 假设表结构为: table_log(time, level, thread_id, file, line, msg)
        std::string ss;
        ss << "INSERT INTO table_log (log_time, log_level, thread_id, file_name, line, msg) VALUES ("
           << "'" << timeStr << "', "                       // 时间
           << "'" << LogLevel::ToString(level) << "', "     // 级别
           << event->getThreadId() << ", "                  // 线程ID
           << event->getFile() << "', "                     // 文件名
           << event->getLine() << ", "
           << "'" << "Message" << "'" << ");";
        
        // 调用模型私有函数执行
        simulate_db_execute(ss.str());
    }
    // 3. reopen 函数实现
    bool SqlLogAppender::reopen() {
    // 模拟重新连接数据库逻辑
    // 如果连接成功返回 true
        std::cout << "[Reopen] Reconnecting to DB..." << std::endl;
        return true; 
    }
    // 5. 私有辅助函数实现
    void SqlLogAppender::simulate_db_execute(const std::string& sql) {
    // 简单打印 SQL 语句模拟入库
        std::cout << "[SQL EXEC] " << sql << std::endl;
    }
}// sylar

