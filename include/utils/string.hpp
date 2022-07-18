#ifndef KZN_UTILS_STRING_HPP
#define KZN_UTILS_STRING_HPP

#include <vector>
#include <string_view>

namespace kzn {

    //template<typename T>
    //struct StaticVec {
    //    size_t size;
    //    T*     data;
//
    //    StaticVec(size_t _size)
    //        : data(new T[_size]), size(_size) {}
//
    //    //~StaticVec() { delete data; }
    //};
    
    std::vector<std::string_view> split(const std::string& cmd, char separator = ' ');
}

#endif // KZN_UTILS_STRING_HPP