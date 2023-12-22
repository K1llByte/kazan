#pragma once

#include <chrono>

namespace kzn {

//! Returns delta time in seconds from the last time delta_time was called.
inline float delta_time() {
    static auto begin = std::chrono::high_resolution_clock::now();
    const auto end = std::chrono::high_resolution_clock::now();
    const float delta =
        std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
    begin = end;
    return delta;
}

} // namespace kzn
