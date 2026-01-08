#pragma once

#include <filesystem>
#include <string_view>

namespace kzn {

//! Generic loading error
struct LoadingError {
    std::string_view message;
};

//! Concept for loadable resource types
template<typename T>
concept LoadableResource = requires(const std::filesystem::path& path) {
    { T::load(path) } -> std::convertible_to<std::shared_ptr<T>>;
};

} // namespace kzn