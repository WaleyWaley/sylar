#pragma once
#include "logger/Logger.h"
#include "logger/singleton.h"

namespace sylar{
    /**
    * @brief 日志器管理类
    */
    class LoggerManager{
    public:
        /**
         * @brief 构造函数
         */
        LoggerManager();
        /**
        * @brief 初始化，主要是结合配置模块实现日志模块初始化
        */
        void init();
        /**
        * @brief 获取指定名称的日志器
        */
        Logger::ptr getLogger(const std::string &name);
        /**
        * @brief 获取root日志器，等效于getLogger("root")
        */
        Logger::ptr getRoot() const { return root_; }
        std::string toYamlString();
    private:
        /// 日志器集合
        std::map<std::string, Logger::ptr> loggers_;
        /// root日志器
        Logger::ptr root_;
    };
    /// 日志器管理类单例
    typedef Singleton<LoggerManager> LoggerMgr;
} // sylar