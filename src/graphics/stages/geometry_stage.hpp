#pragma once

#include "graphics/mesh.hpp"
#include "graphics/renderer.hpp"
#include "graphics/stages/render_stage.hpp"
#include "math/transform.hpp"
#include "render_stage.hpp"
#include "vk/functions.hpp"
#include "vk/pipeline_builder.hpp"
#include "graphics/light.hpp"

#include <glm/trigonometric.hpp>
#include <vulkan/vulkan_core.h>

namespace kzn {

const Vec3 red =    Vec3{0.88, 0.22, 0.22};
const Vec3 green =  Vec3{0.22, 0.88, 0};
const Vec3 blue =   Vec3{0.22, 0.22, 0.88};
const Vec3 cyan =   Vec3{0.22, 0.88, 0.88};
const Vec3 pink =   Vec3{0.88, 0.22, 0.88};
const Vec3 yellow = Vec3{0.88, 0.88, 0.22};

class GeometryStage : public RenderStage {
public:
    // Ctor
    GeometryStage(
        Renderer& renderer,
        vk::RenderPass& render_pass,
        vk::DescriptorSet& camera_dset
    )
        : m_renderer_ptr{&renderer}
        , m_pipeline{
            vk::PipelineBuilder(render_pass)
                .set_vertex_stage(load_shader("shaders://geom.vert.spv"))
                .set_fragment_stage(load_shader("shaders://geom.frag.spv"))
                .set_vertex_input<Vec3,Vec3,Vec2,Vec3>()
                .set_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                .set_cull_mode(VK_CULL_MODE_BACK_BIT)
                .set_front_face(VK_FRONT_FACE_COUNTER_CLOCKWISE)
                .set_depth_test(true)
                .set_depth_write(true)
                .build(renderer.device())
        }
        , m_light_ubo(renderer.device(), sizeof(Lights))
        , m_camera_dset_ptr{&camera_dset}
        , m_light_dset{renderer.device().dset_allocator().allocate(
            *m_pipeline.dset_layout(1)
        )}
    {
        constexpr Vec3 light_color = Vec3{0.8,0.2,0.2};
        constexpr float intensity = 0.5f;
        auto light0 = spot_light(
            intensity,
            Vec3{0,0,0},
            Vec3{1,0,0}, 
            10.f,
            glm::radians(35.f),
            glm::radians(40.f),
            // Vec3{0.8,0.2,0.2}
            Vec3{1}
        );
        auto light1 = spot_light(
            intensity,
            Vec3{0,0,0},
            Vec3{-1,0,0}, 
            10.f,
            glm::radians(35.f),
            glm::radians(45.f),
            // Vec3{0.2,0.2,0.8}
            Vec3{1}
        );
        auto light2 = spot_light(
            intensity,
            Vec3{0,0,0},
            Vec3{0,0,1}, 
            10.f,
            glm::radians(35.f),
            glm::radians(45.f),
            // Vec3{0.2,0.8,0.2}
            Vec3{1}
        );
        auto light3 = spot_light(
            intensity,
            Vec3{0,0,0},
            Vec3{0,0,-1}, 
            10.f,
            glm::radians(35.f),
            glm::radians(45.f),
            // Vec3{0.2,0.8,0.8}
            Vec3{1}
        );
        m_light_ubo.upload(Lights{
            .count = 4,
            .data = {light0, light1, light2, light3},
        });
        m_light_dset.update({m_light_ubo.info()});
    }

    void render(Scene& scene, vk::CommandBuffer& cmd_buffer) override {
        vk::cmd_bind_pipeline(cmd_buffer, m_pipeline);
        const auto swapchain_extent = m_renderer_ptr->swapchain().extent();
        vk::cmd_set_viewport(cmd_buffer, vk::create_viewport(swapchain_extent));
        vk::cmd_set_scissor(cmd_buffer, vk::create_scissor(swapchain_extent));
        vk::cmd_bind_dsets(
            cmd_buffer,
            std::array{m_camera_dset_ptr, &m_light_dset},
            m_pipeline.layout()
        );

        auto meshes_view = scene.registry.registry().view<MeshComponent>();
        for (auto [entity, mesh] : meshes_view->each()) {
            // Bind buffers
            vk::cmd_bind_vtx_buffer(cmd_buffer, mesh.mesh().vtx_buffer());
            vk::cmd_bind_idx_buffer(cmd_buffer, mesh.mesh().idx_buffer());
            
            struct TransformPushData {
                glsl::Mat4 matrix = {1.f};
            } transform;

            auto transform_ptr = scene.registry.registry().try_get<Transform3DComponent>(entity);
            if (transform_ptr != nullptr) {
                transform.matrix = transform_ptr->matrix();
            }
            vk::cmd_push_constants(cmd_buffer, m_pipeline.layout(), transform);
            vk::cmd_draw_indexed(cmd_buffer, mesh.mesh().idx_count());
        }
    }

private:
    Renderer* m_renderer_ptr;
    vk::Pipeline m_pipeline;
    vk::UniformBuffer m_light_ubo;
    vk::DescriptorSet* m_camera_dset_ptr;
    vk::DescriptorSet m_light_dset;
};

} // namespace kzn