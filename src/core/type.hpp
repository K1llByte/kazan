#pragma once

#include "entt/core/type_traits.hpp"
#include <functional>

namespace kzn {

template<typename T>
using Ref = std::reference_wrapper<T>;

template<typename... Ts>
struct TypeList {};

} // namespace kzn