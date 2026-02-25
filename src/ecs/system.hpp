#pragma once

#include "ecs/context.hpp"
#include "ecs/scene.hpp"

namespace kzn {

//! Base class for systems implementations.
struct System {
    virtual ~System() = default;

    virtual void update(Scene& scene, float delta_time) {}

    template<typename T>
    [[nodiscard]]
    T& context() {
        return Context<T>::get();
    }
};

} // namespace kzn