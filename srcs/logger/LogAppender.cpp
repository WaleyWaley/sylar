#include "logger/LogAppender.h"
#include <iostream>

using namespace sylar;


FileLogAppender::FileLogAppender(const std::string& filename) : filename_(filename){}



void StdoutLogAppender::log(LogEvent::ptr event){
    std::cout << formatter_->format(event);
}


void FileLogAppender::log(LogEvent::ptr event){}