#pragma once

#include "ecs/entity.hpp"

namespace kzn {

//! Scene abstraction.
//! NOTE: For now it just owns the ECS registry, but in thee future this will
//! be the serializable object of the scene/world.
struct Scene {
    Registry registry;
};

} // namespace kzn