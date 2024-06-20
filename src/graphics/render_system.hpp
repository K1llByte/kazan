#pragma once

#include "ecs/system.hpp"
#include "graphics/passes/test_pass.hpp"
#include "graphics/renderer.hpp"
#include "graphics/utils.hpp"
#include "vk/functions.hpp"
#include "vk/pipeline.hpp"
#include <memory>

namespace kzn {

struct SpriteComponent {
    // TODO: Texture
};

//! ECS System for rendering
//! This system owns the RenderGraph of passes that will present to screen
class RenderSystem : public System {
public:
    std::vector<std::unique_ptr<Pass>> passes;

public:
    // Ctor
    RenderSystem() = default;
    // Copy
    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;
    // Move
    RenderSystem(RenderSystem&&) = delete;
    RenderSystem& operator=(RenderSystem&&) = delete;
    // Dtor
    ~RenderSystem();

    void update(float delta_time) override;
};

} // namespace kzn