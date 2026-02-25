#pragma once

#include "ecs/scene.hpp"
#include "vk/cmd_buffer.hpp"

namespace kzn {

struct RenderStage {
    virtual ~RenderStage() {}
    virtual void pre_render(Scene& scene) {}
    virtual void render(Scene& scene, vk::CommandBuffer& cmd_buffer) = 0;
    virtual void post_render(Scene& scene) {}
};

} // namespace kzn