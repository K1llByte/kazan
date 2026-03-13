#pragma once

#include "graphics/mesh.hpp"
#include "graphics/renderer.hpp"
#include "graphics/stages/render_stage.hpp"
#include "render_stage.hpp"
#include "vk/functions.hpp"
#include "vk/pipeline_builder.hpp"
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
                .set_vertex_input<Vec3,Vec3,Vec3>()
                .set_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                .set_cull_mode(VK_CULL_MODE_BACK_BIT)
                .set_front_face(VK_FRONT_FACE_COUNTER_CLOCKWISE)
                .set_depth_test(true)
                .set_depth_write(true)
                .build(renderer.device())
        }
        , m_camera_dset_ptr{&camera_dset}
        // , m_mesh{
        //     renderer.device(), 
        //     // Custom Model
        //     *g_resources.load<MeshData>("models://damaged_helmet.glb")

            // Triangle
            // std::vector{
            //     {Vec3{ 0.0, -0.5, 0}},
            //     {Vec3{ 0.5,  0.5, 0}},
            //     {Vec3{-0.5,  0.5, 0}},
            // }
            
            // Cube
            // std::vector<Vertex3D>{
            //     // Right (+X)
            //     {Vec3{ 0.5f, -0.5f,  0.5f}, Vec3{1,0,0}, red},
            //     {Vec3{ 0.5f, -0.5f, -0.5f}, Vec3{1,0,0}, red},
            //     {Vec3{ 0.5f,  0.5f, -0.5f}, Vec3{1,0,0}, red},
            //     {Vec3{ 0.5f,  0.5f, -0.5f}, Vec3{1,0,0}, red},
            //     {Vec3{ 0.5f,  0.5f,  0.5f}, Vec3{1,0,0}, red},
            //     {Vec3{ 0.5f, -0.5f,  0.5f}, Vec3{1,0,0}, red},
            //     // Top (+Y)
            //     {Vec3{-0.5f,  0.5f,  0.5f}, Vec3{0,1,0}, green},
            //     {Vec3{ 0.5f,  0.5f,  0.5f}, Vec3{0,1,0}, green},
            //     {Vec3{ 0.5f,  0.5f, -0.5f}, Vec3{0,1,0}, green},
            //     {Vec3{ 0.5f,  0.5f, -0.5f}, Vec3{0,1,0}, green},
            //     {Vec3{-0.5f,  0.5f, -0.5f}, Vec3{0,1,0}, green},
            //     {Vec3{-0.5f,  0.5f,  0.5f}, Vec3{0,1,0}, green},
            //     // Front (+Z)
            //     {Vec3{-0.5f, -0.5f,  0.5f}, Vec3{0,0,1}, blue},
            //     {Vec3{ 0.5f, -0.5f,  0.5f}, Vec3{0,0,1}, blue},
            //     {Vec3{ 0.5f,  0.5f,  0.5f}, Vec3{0,0,1}, blue},
            //     {Vec3{ 0.5f,  0.5f,  0.5f}, Vec3{0,0,1}, blue},
            //     {Vec3{-0.5f,  0.5f,  0.5f}, Vec3{0,0,1}, blue},
            //     {Vec3{-0.5f, -0.5f,  0.5f}, Vec3{0,0,1}, blue},
            //     // Left (-X)
            //     {Vec3{-0.5f, -0.5f, -0.5f}, Vec3{-1,0,0}, yellow},
            //     {Vec3{-0.5f, -0.5f,  0.5f}, Vec3{-1,0,0}, yellow},
            //     {Vec3{-0.5f,  0.5f,  0.5f}, Vec3{-1,0,0}, yellow},
            //     {Vec3{-0.5f,  0.5f,  0.5f}, Vec3{-1,0,0}, yellow},
            //     {Vec3{-0.5f,  0.5f, -0.5f}, Vec3{-1,0,0}, yellow},
            //     {Vec3{-0.5f, -0.5f, -0.5f}, Vec3{-1,0,0}, yellow},
            //     // Bottom (-Y)
            //     {Vec3{-0.5f, -0.5f, -0.5f}, Vec3{0,-1,0}, pink},
            //     {Vec3{ 0.5f, -0.5f, -0.5f}, Vec3{0,-1,0}, pink},
            //     {Vec3{ 0.5f, -0.5f,  0.5f}, Vec3{0,-1,0}, pink},
            //     {Vec3{ 0.5f, -0.5f,  0.5f}, Vec3{0,-1,0}, pink},
            //     {Vec3{-0.5f, -0.5f,  0.5f}, Vec3{0,-1,0}, pink},
            //     {Vec3{-0.5f, -0.5f, -0.5f}, Vec3{0,-1,0}, pink},
            //     // Back (-Z)
            //     {Vec3{ 0.5f, -0.5f, -0.5f}, Vec3{0,0,-1}, cyan},
            //     {Vec3{-0.5f, -0.5f, -0.5f}, Vec3{0,0,-1}, cyan},
            //     {Vec3{-0.5f,  0.5f, -0.5f}, Vec3{0,0,-1}, cyan},
            //     {Vec3{-0.5f,  0.5f, -0.5f}, Vec3{0,0,-1}, cyan},
            //     {Vec3{ 0.5f,  0.5f, -0.5f}, Vec3{0,0,-1}, cyan},
            //     {Vec3{ 0.5f, -0.5f, -0.5f}, Vec3{0,0,-1}, cyan},
            // }
        // }
    {
    }

    void render(Scene& scene, vk::CommandBuffer& cmd_buffer) override {
        vk::cmd_bind_pipeline(cmd_buffer, m_pipeline);
        const auto swapchain_extent = m_renderer_ptr->swapchain().extent();
        vk::cmd_set_viewport(cmd_buffer, vk::create_viewport(swapchain_extent));
        vk::cmd_set_scissor(cmd_buffer, vk::create_scissor(swapchain_extent));
        vk::cmd_bind_dsets(
            cmd_buffer,
            std::array{m_camera_dset_ptr},
            m_pipeline.layout()
        );

        auto meshes_view = scene.registry.registry().view<MeshComponent>();
        for (auto [_, mesh] : meshes_view->each()) {
            vk::cmd_bind_vtx_buffer(cmd_buffer, mesh.mesh().vtx_buffer());
            vk::cmd_bind_idx_buffer(cmd_buffer, mesh.mesh().idx_buffer());
            vk::cmd_draw_indexed(cmd_buffer, mesh.mesh().idx_count());
        }
    }

private:
    Renderer* m_renderer_ptr;
    vk::Pipeline m_pipeline;
    vk::DescriptorSet* m_camera_dset_ptr;
};

} // namespace kzn