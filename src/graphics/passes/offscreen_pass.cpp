#include "offscreen_pass.hpp"
#include "graphics/render_image.hpp"
#include "graphics/renderer.hpp"
#include "graphics/utils.hpp"
#include "math/transform.hpp"
#include <vulkan/vulkan_core.h>

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

OffscreenPass::OffscreenPass(RenderImage& render_image, glm::vec3& clear_color)
    : m_render_image{render_image}
    , m_clear_color(clear_color)
    , m_offscreen_render_pass(
          // TODO: Grab format from image handle
          simple_offscreen_pass(
              Renderer::device(),
              Renderer::swapchain().image_format()
          )
      )
    , m_framebuffer(
          m_offscreen_render_pass,
          {render_image.image_view()},
          m_render_image.extent()
      )
    , m_pipeline(triangle_pipeline(m_offscreen_render_pass)) {
}

void OffscreenPass::render(vk::CommandBuffer& cmd_buffer) {

    VkClearValue clear_color{
        {{m_clear_color.r, m_clear_color.y, m_clear_color.z, 1.0f}}
    };
    // VkClearValue clear_color{{{0.01f, 0.01f, 0.01f, 1.0f}}};

    // Begin Render Pass
    m_offscreen_render_pass.begin(cmd_buffer, m_framebuffer, {clear_color});

    auto view = Registry::registry.view<Transform2DComponent>();
    view.each([&](auto entity, Transform2DComponent& t) {
        // Draw Commands
        vk::cmd_set_viewport(
            cmd_buffer, vk::create_viewport(m_render_image.extent())
        );
        vk::cmd_set_scissor(
            cmd_buffer, vk::create_scissor(m_render_image.extent())
        );

        m_pipeline.bind(cmd_buffer);
        vk::cmd_draw(cmd_buffer, 3);
    });

    // End Render Pass
    m_offscreen_render_pass.end(cmd_buffer);
}

} // namespace kzn