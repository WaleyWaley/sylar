#!/bin/bash

# 1. 编译所有的 .cpp 文件
# -I sylar/include : 告诉编译器去 sylar/include 目录下找头文件
# sylar/srcs/logger/*.cpp : 把 logger 目录下所有的实现文件都编译进去

g++ testlogger.cpp srcs/logger/*.cpp \
    -std=c++20 \
    -I include \
    -o testlogger

# 2. 运行
./testlogger