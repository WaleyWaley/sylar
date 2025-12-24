# 编译器设置
CXX = g++

# 编译选项：
# -std=c++11: 使用 C++11 标准
# -g: 生成调试信息 (方便 GDB 调试)
# -Wall: 开启大部分警告
CXXFLAGS = -std=c++11 -g -Wall

# 头文件路径 (如果有其他头文件目录，加在这里)
# -I. 表示搜索当前目录
INCLUDES = -I.

# 链接库设置 (关键点！)
# -lyaml-cpp: 告诉链接器链接 yaml-cpp 库
# -lpthread: 服务器开发通常都需要线程库，习惯性加上
LIBS = -lyaml-cpp -lpthread

# 目标文件名
TARGET = test_config

# 源文件
SRC = test_config.cpp

# --- 下面是编译规则 ---

# 默认目标 (直接敲 make 时执行)
all: $(TARGET)

# 生成可执行文件
# $@ 代表目标文件 (test_config)
# $^ 代表所有依赖文件 (test_config.cpp)
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# 清理规则 (执行 make clean 时删除生成的文件)
clean:
	rm -f $(TARGET)