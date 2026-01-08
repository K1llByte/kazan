#pragma once

#include <algorithm>
#include <cstdint>
#include <string_view>
#include <vector>

namespace kzn {

// TODO: Optimize this function
template<std::size_t N>
[[nodiscard]]
constexpr std::vector<std::string_view> split(
    std::string_view str,
    const std::array<char, N>& separators
) {
    uint32_t num_sparator = 0;
    for (uint32_t i = 0; i < str.length(); ++i)
        if (!std::ranges::contains(separators, str[i]) &&
            (i == 0 || std::ranges::contains(separators, str[i - 1]))) {
            ++num_sparator;
        }

    std::vector<std::string_view> res(num_sparator);
    uint32_t idx = 0;
    uint32_t begin = 0;
    uint32_t end = 0;
    for (uint32_t i = 0; i < str.length(); ++i) {
        if (std::ranges::contains(separators, str[i]) || str[i] == '\n') {
            if (end != begin) {
                std::string_view tmp(str.data(), end);
                tmp.remove_prefix(begin);
                res[idx++] = tmp;
            }
            begin = ++end;
        }
        else {
            ++end;
        }
    }
    std::string_view tmp(str.data(), end);
    tmp.remove_prefix(begin);

    if (!tmp.empty())
        res[idx] = tmp;
    return res;
}

} // namespace kzn