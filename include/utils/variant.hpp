#ifndef KZN_UTILS_TIME_HPP
#define KZN_UTILS_TIME_HPP

#include <variant>

// helper type for the visitor #4
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace kzn {
    template<typename ...V>
    struct Variant: public std::variant<V...> {
        //template<typename F, typename ...Args>
        //void match()
    };
} // namespace kzn

#endif // KZN_UTILS_TIME_HPP