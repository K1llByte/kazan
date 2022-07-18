#ifndef KZN_UTILS_STRING_HPP
#define KZN_UTILS_STRING_HPP

#include <vector>
#include <string_view>

namespace kzn {

    // FIXME: This should be string_view but for some
    // reason doesn't work properly
    std::vector<std::string> split(const std::string& cmd, char separator = ' ');
}

#endif // KZN_UTILS_STRING_HPP