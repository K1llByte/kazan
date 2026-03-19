#pragma once

#include "graphics/renderer.hpp"
#include "graphics/stages/render_stage.hpp"
#include "graphics/stages/sprite_stage.hpp"
#include "graphics/texture.hpp"
#include "resources/resources.hpp"
#include "vk/dset_layout.hpp"
#include "vk/functions.hpp"
#include "vk/image.hpp"
#include "vk/pipeline.hpp"
#include "vk/pipeline_builder.hpp"
#include "vk/render_pass.hpp"

namespace kzn {

class PlanetStage : public RenderStage {
public:
    // Ctor
    PlanetStage(
        Renderer& renderer,
        vk::RenderPass& render_pass,
        vk::DescriptorSet& camera_dset
    )
        : m_renderer_ptr{&renderer}
        , m_pipeline{
            vk::PipelineBuilder(render_pass)
                .set_vertex_stage(load_shader("shaders://planet/planet.vert.spv"))
                .set_fragment_stage(load_shader("shaders://planet/planet.frag.spv"))
                .set_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
                .set_depth_test(true)
                .set_depth_write(true)
                .set_depth_cmp(VK_COMPARE_OP_LESS_OR_EQUAL)
                .build(renderer.device())
        }
        , m_camera_dset_ptr{&camera_dset}
        , m_earth_tex_ptr{g_resources.load<TextureData>("textures://earth.jpg")}
        , m_earth_dset{renderer.device().dset_allocator().allocate(
            *m_pipeline.dset_layout(1)
        )}
        , m_earth_image(renderer.device(), m_earth_tex_ptr->vk_extent())
    {
        // Upload texture data to gpu image memory
        m_earth_image.upload(m_earth_tex_ptr->bytes);
        // Update dset and upload data
        m_earth_dset.update({m_earth_image.info()});
    }

    void render(Scene& scene, vk::CommandBuffer& cmd_buffer) override {
        vk::cmd_bind_pipeline(cmd_buffer, m_pipeline);
        const auto swapchain_extent = m_renderer_ptr->swapchain().extent();
        vk::cmd_set_viewport(cmd_buffer, vk::create_viewport(swapchain_extent));
        vk::cmd_set_scissor(cmd_buffer, vk::create_scissor(swapchain_extent));

        vk::cmd_bind_dsets(
            cmd_buffer,
            std::array{m_camera_dset_ptr, &m_earth_dset},
            m_pipeline.layout()
        );

        vk::cmd_draw(cmd_buffer, 4);
    }

private:
    Renderer* m_renderer_ptr;
    vk::Pipeline m_pipeline;
    vk::DescriptorSet* m_camera_dset_ptr;
    std::shared_ptr<TextureData> m_earth_tex_ptr;
    vk::DescriptorSet m_earth_dset;
    vk::Image m_earth_image;
};

} // namespace kzn