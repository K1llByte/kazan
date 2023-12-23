#pragma once

#include <chrono>

namespace kzn {

//! Returns delta time in seconds from the last time delta_time was called.
inline float delta_time() {
    static auto begin = std::chrono::high_resolution_clock::now();
    const auto end = std::chrono::high_resolution_clock::now();
    auto microseconds =
        std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
            .count();
    begin = end;
    // FIXME: Bad conversion to seconds
    return float(microseconds) / 1000000.f;
}

} // namespace kzn
