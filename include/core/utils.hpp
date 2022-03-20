#ifndef KZN_UTILS_HPP
#define KZN_UTILS_HPP

#include <algorithm>

#define KZN_IMPL_ERROR(name, msg)             \
    class name: public std::exception         \
    {                                         \
    public:                                   \
        name() = default;                     \
        ~name() = default;                    \
        const char* what() const noexcept     \
        { return msg; }                       \
    };

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