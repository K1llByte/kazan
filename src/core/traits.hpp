#pragma once

#include <tuple>
#include <type_traits>

namespace kzn {

template<typename T, typename... U>
struct is_any_of : std::bool_constant<(std::is_same_v<T, U> || ...)> {};

template<class T, class... U>
constexpr bool is_any_of_v = is_any_of<T, U...>::value;

//! Helper type trait struct that specifies return type and arguments of
//! invocables.
template<typename, typename = std::void_t<>>
struct FunctionTraits {};

template<typename Return, typename... Args>
struct FunctionTraits<Return (*)(Args...)> {
    using ReturnType = Return;
    using ArgTypes = std::tuple<Args...>;
};

template<typename Return, typename Class, typename... Args>
struct FunctionTraits<Return (Class::*)(Args...)> {
    using ReturnType = Return;
    using ArgTypes = std::tuple<Args...>;
};

template<typename Return, typename Class, typename... Args>
struct FunctionTraits<Return (Class::*)(Args...) const> {
    using ReturnType = Return;
    using ArgTypes = std::tuple<Args...>;
};

template<typename F>
struct FunctionTraits<F, std::void_t<decltype(&F::operator())>> {
    using ReturnType = FunctionTraits<decltype(&F::operator())>::ReturnType;
    using ArgTypes = FunctionTraits<decltype(&F::operator())>::ArgTypes;
};
} // namespace kzn