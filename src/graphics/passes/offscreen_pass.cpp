#include "offscreen_pass.hpp"
#include "graphics/render_image.hpp"
#include "graphics/renderer.hpp"
#include "graphics/utils.hpp"
#include "math/transform.hpp"
#include "vk/dset_layout.hpp"
#include <vulkan/vulkan_core.h>

namespace kzn {

OffscreenPass::OffscreenPass(RenderImage& render_image, Vec3& clear_color)
    : m_clear_color(clear_color)
    , m_render_image{render_image}
    , m_offscreen_render_pass(
          simple_offscreen_pass(Renderer::device(), m_render_image.format())
      )
    , m_framebuffer(
          m_offscreen_render_pass,
          {render_image.image_view()},
          m_render_image.extent()
      )
    , m_tmp_ubo(Renderer::device(), sizeof(SpriteComponent))
    , m_dset(Renderer::dset_allocator().allocate(
          Renderer::dset_layout_cache().create_layout({
              vk::uniform_binding(0),
          })
      ))
    , m_pipeline(
          Renderer::device(),
          vk::PipelineStages{
              .vertex = "assets/shaders/triangle/triangle_test.vert.spv",
              .fragment = "assets/shaders/triangle/triangle.frag.spv",
          },
          vk::PipelineConfig(m_offscreen_render_pass)
              .set_layout(
                  vk::PipelineLayout{.descriptor_sets = {m_dset.layout()}}
              )
      ) {
    Log::debug("Created OffscreenPass");
    // Update all descriptor sets with
    m_dset.update({m_tmp_ubo.info()});
}

void OffscreenPass::render(vk::CommandBuffer& cmd_buffer) {

    VkClearValue clear_color{
        {{m_clear_color.r, m_clear_color.y, m_clear_color.z, 1.0f}}
    };

    // Begin Render Pass
    m_offscreen_render_pass.begin(cmd_buffer, m_framebuffer, {clear_color});

    auto view = Registry::registry.view<SpriteComponent>();
    view.each([&](auto entity, SpriteComponent& sc) {
        // Draw Commands
        vk::cmd_set_viewport(
            cmd_buffer, vk::create_viewport(m_render_image.extent())
        );
        vk::cmd_set_scissor(
            cmd_buffer, vk::create_scissor(m_render_image.extent())
        );

        m_tmp_ubo.upload(&sc.params);

        m_pipeline.bind(cmd_buffer);
        m_dset.bind(cmd_buffer, m_pipeline.layout());
        vk::cmd_draw(cmd_buffer, 3);
    });

    // End Render Pass
    m_offscreen_render_pass.end(cmd_buffer);
}

} // namespace kzn