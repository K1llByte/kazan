#pragma once

#include "core/type.hpp"
#include "ecs/entity.hpp"
#include "graphics/renderer.hpp"
#include "graphics/sprite_component.hpp"
#include "graphics/stages/render_stage.hpp"
#include "math/transform.hpp"
#include "vk/dset.hpp"
#include "vk/functions.hpp"
#include "vk/pipeline.hpp"
#include "vk/pipeline_builder.hpp"
#include "vk/render_pass.hpp"

namespace kzn {

class SpriteStage : public RenderStage {
public:
    // Ctor
    SpriteStage(Renderer& renderer, vk::RenderPass& render_pass, vk::DescriptorSet& camera_dset)
        : m_renderer_ptr{&renderer}
        , m_sprite_geom_cache{renderer}
        , m_pipeline{vk::PipelineBuilder(render_pass)
            .set_vertex_stage(load_shader("shaders://sprites/sprite_render.vert.spv"))
            .set_fragment_stage(load_shader("shaders://sprites/sprite_render.frag.spv"))
            .set_vertex_input<Vec3, Vec2>()
            .set_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
            .build(renderer.device())
        }
        , m_camera_dset_ptr{&camera_dset}
        {}
    // Copy
    SpriteStage(const SpriteStage&) = delete;
    SpriteStage& operator=(const SpriteStage&) = delete;
    // Move
    SpriteStage(SpriteStage&&) = delete;
    SpriteStage& operator=(SpriteStage&&) = delete;
    // Dtor
    ~SpriteStage() {
        // Destroy all geometry data left
        m_sprite_geom_cache.clear();
    }

    void pre_render(Scene& scene) override {
        auto sprites_view = scene.registry.registry().view<SpriteComponent>();
        for (auto [entity, sprite] : sprites_view->each()) {
            if (sprite.geometry() == nullptr) {
                sprite.create_geometry(m_sprite_geom_cache);
            }

            if (!sprite.material()->has_render_data()) {
                sprite.material()->create_render_data(*m_renderer_ptr);
            }

            // Properties of materials can be changed multiple times per frame
            // but uploading those changes is only done once. update_render_data
            // checks if the material was modified before uploading data to the
            // GPU.
            sprite.material()->update_render_data();
        }
    }

    void render(Scene& scene, vk::CommandBuffer& cmd_buffer) override {
        vk::cmd_bind_pipeline(cmd_buffer, m_pipeline);

        const auto swapchain_extent = m_renderer_ptr->swapchain().extent();
        vk::cmd_set_viewport(
            cmd_buffer, vk::create_viewport(swapchain_extent)
        );
        vk::cmd_set_scissor(
            cmd_buffer, vk::create_scissor(swapchain_extent)
        );

        // Render sprite components
        auto sprites_view = scene.registry.registry().view<SpriteComponent>();
        for (auto [entity, sprite] : sprites_view.each()) {
            Mat4 transform_mat{};

            // If sprite has transform component, update pvm push constant.
            auto transform_ptr =
                scene.registry.registry().try_get<Transform2DComponent>(entity);
            if (transform_ptr != nullptr) {
                transform_mat = transform_ptr->matrix();
            }

            vk::cmd_bind_dsets(
                cmd_buffer,
                std::array{
                    m_camera_dset_ptr,
                    &sprite.material()->render_data()->material_dset
                },
                m_pipeline.layout()
            );

            struct PvmPushData {
                Mat4 matrix;
            } pvm = {
                transform_mat 
            };
            vk::cmd_push_constants(
                cmd_buffer, m_pipeline.layout(), pvm
            );

            // Bind vertex buffer
            vk::cmd_bind_vtx_buffer(cmd_buffer, sprite.geometry()->quad_vbo);

            // Draw call
            vk::cmd_draw(cmd_buffer, 4);
        }
    }

private:
    Renderer* m_renderer_ptr;
    SpriteGeometryCache m_sprite_geom_cache;
    vk::Pipeline m_pipeline;
    vk::DescriptorSet* m_camera_dset_ptr;
};

} // namespace kzn