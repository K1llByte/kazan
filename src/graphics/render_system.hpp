#pragma once

#include "ecs/system.hpp"
#include "graphics/graphics_context.hpp"
#include "graphics/renderer.hpp"
#include "graphics/utils.hpp"
#include "vk/functions.hpp"
#include "vk/pipeline.hpp"

namespace kzn {

struct SpriteComponent {
    // TODO: Texture
};

//! ECS System for rendering
class RenderSystem : public System {
public:
    // Ctor
    RenderSystem(Window& window);
    // Copy
    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;
    // Move
    RenderSystem(RenderSystem&&) = delete;
    RenderSystem& operator=(RenderSystem&&) = delete;
    // Dtor
    ~RenderSystem() override;

    void update(float delta_time) override;

    void on_swapchain_resize(const SwapchainResize&);

private:
    ScopedGraphicsContext m_scoped_gfx_context;
    Renderer m_renderer;
    vk::RenderPass m_main_render_pass;
    std::vector<vk::Framebuffer> m_framebuffers;
    vk::Pipeline m_pipeline;
};

} // namespace kzn