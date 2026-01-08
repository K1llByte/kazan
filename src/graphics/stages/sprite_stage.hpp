#pragma once

#include "ecs/entity.hpp"
#include "graphics/renderer.hpp"
#include "graphics/sprite_component.hpp"
#include "graphics/stages/render_stage.hpp"
#include "math/transform.hpp"
#include "vk/dset.hpp"
#include "vk/functions.hpp"
#include "vk/pipeline.hpp"
#include "vk/render_pass.hpp"

namespace kzn {

struct PvmPushData {
    glsl::Mat4 matrix = {};
};

class SpriteStage : public RenderStage {
public:
    // Ctor
    SpriteStage(vk::RenderPass& render_pass, vk::DescriptorSet& camera_dset)
        : m_pipeline{
              Renderer::device(),
              vk::PipelineStages{
                  .vertex = "assets/shaders/sprites/sprite_render.vert.spv",
                  .fragment = "assets/shaders/sprites/sprite_render.frag.spv",
              },
              vk::PipelineConfig(render_pass)
                  .set_vertex_input<Vec3, Vec2>()
                  .set_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
                  .set_layout(vk::PipelineLayout{
                      .push_constants = {vk::push_constant_range<PvmPushData>()
                      },
                      .descriptor_sets =
                          {Renderer::dset_layout_cache()
                               .create_layout({vk::uniform_binding(0)}),
                           Renderer::dset_layout_cache().create_layout(
                               {vk::sampler_binding(0), vk::uniform_binding(1)}
                           )}
                  })
          }
        , m_camera_dset{camera_dset} {}
    // Copy
    SpriteStage(const SpriteStage&) = delete;
    SpriteStage& operator=(const SpriteStage&) = delete;
    // Move
    SpriteStage(SpriteStage&&) = delete;
    SpriteStage& operator=(SpriteStage&&) = delete;
    // Dtor
    ~SpriteStage() {
        // Make sure all resources are released
        // FIXME: This is only a problem because of default material, think of a
        // way to fix that
        auto sprites_view = Registry::registry().view<SpriteComponent>();
        for (auto [entity, sprite] : sprites_view->each()) {
            sprite.material()->destroy_render_data();
        }

        // Destroy all geometry data left
        m_sprite_geom_cache.clear();
    }

    void pre_render() override {
        auto sprites_view = Registry::registry().view<SpriteComponent>();
        for (auto [entity, sprite] : sprites_view->each()) {
            if (sprite.geometry() == nullptr) {
                sprite.create_geometry(m_sprite_geom_cache);
            }

            if (!sprite.material()->has_render_data()) {
                sprite.material()->create_render_data();
            }

            // Properties of materials can be changed multiple times per frame
            // but uploading those changes is only done once. update_render_data
            // checks if the material was modified before uploading data to the
            // GPU.
            sprite.material()->update_render_data();
        }
    }

    void render(vk::CommandBuffer& cmd_buffer) override {
        m_pipeline.bind(cmd_buffer);

        vk::cmd_set_viewport(
            cmd_buffer, vk::create_viewport(Renderer::swapchain().extent())
        );
        vk::cmd_set_scissor(
            cmd_buffer, vk::create_scissor(Renderer::swapchain().extent())
        );

        // Render sprite components
        auto sprites_view = Registry::registry().view<SpriteComponent>();
        for (auto [entity, sprite] : sprites_view.each()) {
            Mat4 transform_mat{};

            // If sprite has transform component, update pvm push constant.
            auto transform_ptr =
                Registry::registry().try_get<Transform2DComponent>(entity);
            if (transform_ptr != nullptr) {
                transform_mat = transform_ptr->matrix();
            }

            vk::cmd_bind_dsets(
                std::array{
                    Ref(m_camera_dset),
                    Ref(sprite.material()->render_data()->material_dset)
                },
                cmd_buffer,
                m_pipeline.layout()
            );

            vk::cmd_push_constants(
                cmd_buffer, m_pipeline.layout(), PvmPushData{transform_mat}
            );

            // Bind vertex buffer
            sprite.geometry()->quad_vbo.bind(cmd_buffer);

            // Draw call
            vk::cmd_draw(cmd_buffer, 4);
        }
    }

private:
    SpriteGeometryCache m_sprite_geom_cache;
    vk::Pipeline m_pipeline;
    Ref<vk::DescriptorSet> m_camera_dset;
};

} // namespace kzn