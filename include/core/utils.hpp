#ifndef KZN_UTILS_HPP
#define KZN_UTILS_HPP

// #include <cstdint>
#include <algorithm>

namespace kzn
{
    // Auxiliar structure to allow template string literals
    template<std::size_t N>
    struct StringLiteral {
        constexpr StringLiteral(const char (&str)[N])
        {
            std::copy_n(str, N, value);
        }
        
        char value[N];
    };
}

#endif // KZN_UTILS_HPP