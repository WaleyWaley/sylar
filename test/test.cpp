#include <boost/version.hpp>
#include <iostream>
#include <boost/config.hpp>
#include "boost/lexical_cast.hpp" // 需要包含的头文件
#include <typeinfo>
#include <cxxabi.h> // GCC/Clang 专用，用于把乱码名字变回人话

using namespace std;

template <typename T>
struct ShowType;



auto main()->int{
    string p = "32768";

    auto i = boost::lexical_cast<int>(p);

    cout << ShowType<decltype(i)> << endl;

    return 0;
}
