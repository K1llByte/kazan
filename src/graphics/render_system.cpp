#include "render_system.hpp"
#include "ecs/entity.hpp"
#include "math/transform.hpp"

namespace kzn {

//! \warning This is temporary
inline vk::Pipeline triangle_pipeline(vk::RenderPass& render_pass) {
    return vk::Pipeline(
      render_pass.device(),
      vk::PipelineStages{
        .vertex = "assets/shaders/triangle/triangle.vert.spv",
        .fragment = "assets/shaders/triangle/triangle.frag.spv",
      },
      vk::PipelineConfig(render_pass));
}

RenderSystem::RenderSystem(Window& window)
  : m_scoped_gfx_context(window)
  , m_renderer(GraphicsContext::device(), GraphicsContext::swapchain(), window)
  , m_main_render_pass(simple_pass(GraphicsContext::device(),
                                   GraphicsContext::swapchain().image_format()))
  , m_framebuffers(create_swapchain_framebuffers(m_main_render_pass,
                                                 GraphicsContext::swapchain()))
  , m_pipeline(triangle_pipeline(m_main_render_pass)) {
    m_renderer.on_swapchain_resize([&] {
        m_framebuffers = create_swapchain_framebuffers(
          m_main_render_pass, GraphicsContext::swapchain());
    });
}

RenderSystem::~RenderSystem() {
    GraphicsContext::device().wait_idle();
}

void RenderSystem::update(float delta_time) {
    auto& swapchain = GraphicsContext::swapchain();

    VkClearValue clear_color{ { { 0.01f, 0.01f, 0.01f, 1.0f } } };
    m_renderer.render_frame([&](auto& cmd_buffer) {
        // Begin Render Pass
        m_main_render_pass.begin(cmd_buffer,
                                 m_framebuffers[swapchain.current_index()],
                                 { clear_color });

        auto view = Registry::registry.view<Transform2DComponent>();
        view.each([&](auto entity, Transform2DComponent& t) {
            // Draw Commands
            vk::cmd_set_viewport(cmd_buffer,
                                 vk::create_viewport(swapchain.extent()));
            vk::cmd_set_scissor(cmd_buffer,
                                vk::create_scissor(swapchain.extent()));

            m_pipeline.bind(cmd_buffer);
            vk::cmd_draw(cmd_buffer, 3);
        });

        // End Render Pass
        m_main_render_pass.end(cmd_buffer);
    });
}

}