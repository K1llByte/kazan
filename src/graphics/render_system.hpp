#pragma once

#include "core/log.hpp"
#include "ecs/system.hpp"
#include "graphics/graphics_context.hpp"
#include <cassert>

namespace kzn {

class RenderSystem : public System {
public:
    RenderSystem() { assert(GraphicsContext::exists()); };
    ~RenderSystem() = default;

    void update(float delta_time) override {
        Log::info("RenderSystem::update()");
    }
};

}