#pragma once

#include <fmt/format.h>

#include <charconv>
#include <concepts>
#include <stdexcept>
#include <string_view>

namespace kzn {

template<typename T>
struct ConsoleTypeTraits;

template<typename T>
concept ConsoleType = requires(T a, std::string_view arg) {
    { ConsoleTypeTraits<T>::convert_to(arg) } -> std::convertible_to<T>;
};

template<std::integral I>
struct ConsoleTypeTraits<I> {
    [[nodiscard]]
    static I convert_to(std::string_view arg) {
        I value;
        auto [_, ec] =
            std::from_chars(arg.data(), arg.data() + arg.size(), value);
        if (ec != std::errc()) {
            throw std::runtime_error(
                fmt::format("Cannot convert '{}' to int", arg)
            );
        }
        return value;
    }
};

template<std::floating_point F>
struct ConsoleTypeTraits<F> {
    [[nodiscard]]
    static F convert_to(std::string_view arg) {
        F value;
        auto [_, ec] =
            std::from_chars(arg.data(), arg.data() + arg.size(), value);
        if (ec != std::errc()) {
            throw std::runtime_error(
                fmt::format("Cannot convert '{}' to float", arg)
            );
        }
        return value;
    }
};

} // namespace kzn