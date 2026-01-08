#pragma once

#include "entt/core/type_traits.hpp"
#include <functional>

namespace kzn {

template<typename T>
using Ref = std::reference_wrapper<T>;

template<typename... Types>
using TypeList = entt::type_list<Types...>;

template<typename List, typename Type>
using type_list_contains = entt::type_list_contains<List, Type>;

template<typename List, typename Type>
inline constexpr bool type_list_contains_v = type_list_contains_v<List, Type>;

} // namespace kzn