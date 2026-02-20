#pragma once

#include "ecs/context.hpp"

namespace kzn {

//! Base class for systems implementations.
struct System {
    virtual ~System() = default;

    virtual void update(float delta_time) {}

    template<typename T>
    [[nodiscard]]
    T& context() {
        return Context<T>::get();
    }
};

} // namespace kzn