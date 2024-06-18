#include "test_pass.hpp"
#include "graphics/utils.hpp"
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
        vk::PipelineConfig(render_pass)
    );
}

TestPass::TestPass(Window& window)
    : m_test_render_pass(
          simple_pass(Renderer::device(), Renderer::swapchain().image_format())
      )
    , m_framebuffers(create_swapchain_framebuffers(
          m_test_render_pass,
          Renderer::swapchain()
      ))
    , m_pipeline(triangle_pipeline(m_test_render_pass)) {
    EventManager::listen<SwapchainResize>(
        EventHandler(this, &TestPass::on_swapchain_resize)
    );
}

void TestPass::render(vk::CommandBuffer& cmd_buffer) {
    auto& swapchain = Renderer::swapchain();
    VkClearValue clear_color{{{0.01f, 0.01f, 0.01f, 1.0f}}};

    // Begin Render Pass
    m_test_render_pass.begin(
        cmd_buffer, m_framebuffers[swapchain.current_index()], {clear_color}
    );

    auto view = Registry::registry.view<Transform2DComponent>();
    view.each([&](auto entity, Transform2DComponent& t) {
        // Draw Commands
        vk::cmd_set_viewport(
            cmd_buffer, vk::create_viewport(swapchain.extent())
        );
        vk::cmd_set_scissor(cmd_buffer, vk::create_scissor(swapchain.extent()));

        m_pipeline.bind(cmd_buffer);
        vk::cmd_draw(cmd_buffer, 3);
    });

    // End Render Pass
    m_test_render_pass.end(cmd_buffer);
}

void TestPass::on_swapchain_resize(const SwapchainResize&) {
    m_framebuffers = create_swapchain_framebuffers(
        m_test_render_pass, Renderer::swapchain()
    );
}

} // namespace kzn