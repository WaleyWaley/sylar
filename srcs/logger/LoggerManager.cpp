#include "logger/LoggerManager.h"
#include "logger/LogAppender.h"
#include "logger/singleton.h"

namespace sylar{
    /**
    * @brief 构造函数
    */ 
    LoggerManager::LoggerManager(){
        root_.reset(new Logger("root"));
        root_->addAppender(LogAppender::ptr(new StdoutLogAppender));
        loggers_[root_->getName()] = root_;
        init();
    }

    /**
    * 如果指定名称的日志器未找到，那会就新创建一个，但是新创建的Logger是不带Appender的，
    * 需要手动添加Appender
    */
    Logger::ptr LoggerManager::getLogger(const std::string &name){
        // MutexType::Lock lock(mutex_);
        auto it = loggers_.find(name);
        // 如果找到了该日志器就返回该日志器的智能指针
        if(it != loggers_.end())
            return it->second;
        // 没找到就新创建一个,但是不带Appender需要自己添加
        Logger::ptr logger(new Logger(name));
        loggers_[name] = logger;
        return logger;
    }
    /**
    * @todo 实现从配置文件加载日志配置
    */
    void LoggerManager::init(){}

    // std::string LoggerManager::toYamlString(){
    //     YAML::Node node;
    //     node["name"] = name_;
    //     node["Level"] = LogLevel::ToString(level_);
    //     for(auto& i : appenders_){
    //         node["appenders"].push_back(YAML::Load(i.second->toYamlString()));
    //     }
    //     std::stringstream ss;
    //     ss << node;
    //     return ss.str();
    // }


    
} // sylar