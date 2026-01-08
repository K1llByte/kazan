#pragma once

#include "ecs/context.hpp"
#include "graphics/debug_render.hpp"
#include "graphics/renderer.hpp"
#include "graphics/stages/render_stage.hpp"
#include "graphics/stages/sprite_stage.hpp"
#include "vk/dset.hpp"
#include "vk/pipeline.hpp"
#include "vk/render_pass.hpp"

#include <vector>

namespace kzn {

class DebugStage : public RenderStage {
public:
    // Ctor
    DebugStage(vk::RenderPass& render_pass, vk::DescriptorSet& camera_dset)
        : m_debug_pipeline{
              Renderer::device(),
              vk::PipelineStages{
                  .vertex = "assets/shaders/debug.vert.spv",
                  .fragment = "assets/shaders/debug.frag.spv",
              },
              vk::PipelineConfig(render_pass)
                  .set_vertex_input<Vec2, Vec3>()
                  .set_topology(VK_PRIMITIVE_TOPOLOGY_LINE_LIST)
                  .set_layout(vk::PipelineLayout{
                      .push_constants = {vk::push_constant_range<PvmPushData>()
                      },
                      .descriptor_sets =
                          {Renderer::dset_layout_cache()
                               .create_layout({vk::uniform_binding(0)})}
                  })
          }, m_camera_dset{camera_dset} {}
    // Copy
    DebugStage(const DebugStage&) = delete;
    DebugStage& operator=(const DebugStage&) = delete;
    // Move
    DebugStage(DebugStage&&) = delete;
    DebugStage& operator=(DebugStage&&) = delete;
    // Dtor
    ~DebugStage() {}

    bool is_enabled() const { return m_use_debug_render; }
    void enable(bool enable) { m_use_debug_render = enable; }

    void render(vk::CommandBuffer& cmd_buffer) override {
        // Create debug line render vbo
        auto debug_vbo_opt = m_use_debug_render
                                 ? m_debug_render.create_debug_vbo()
                                 : std::nullopt;

        if (m_use_debug_render && debug_vbo_opt.has_value()) {
            auto& debug_render = m_debug_render.value();
            auto& debug_vbo = debug_vbo_opt.value();
            m_debug_pipeline.bind(cmd_buffer);

            vk::cmd_set_line_width(cmd_buffer, debug_render.line_width());
            vk::cmd_bind_dset(
                m_camera_dset, cmd_buffer, m_debug_pipeline.layout()
            );
            vk::cmd_push_constants(
                cmd_buffer,
                m_debug_pipeline.layout(),
                PvmPushData{
                    // Transform matrix to correct y position to be inverted
                    Mat4{1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}
                }
            );
            debug_vbo.bind(cmd_buffer);
            vk::cmd_draw(cmd_buffer, debug_render.lines().size());

            debug_render.clear();
        }
    }

private:
    vk::Pipeline m_debug_pipeline;
    bool m_use_debug_render = false;
    Ref<vk::DescriptorSet> m_camera_dset;
    // Debug render interface
    Context<DebugRender> m_debug_render;
};

} // namespace kzn