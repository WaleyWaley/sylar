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

    StdoutLogAppender::StdoutLogAppender(){}
    void StdoutLogAppender::log(LogEvent::ptr event){
        std::cout << formatter_->format(event);
    }
}