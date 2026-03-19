#pragma once

#include "graphics/renderer.hpp"
#include "graphics/stages/render_stage.hpp"
#include "graphics/texture.hpp"
#include "resources/resources.hpp"
#include "vk/cube_image.hpp"
#include "vk/dset_layout.hpp"
#include "vk/functions.hpp"
#include "vk/image.hpp"
#include "vk/pipeline.hpp"
#include "vk/pipeline_builder.hpp"
#include "vk/render_pass.hpp"

namespace kzn {

class SkyboxStage : public RenderStage {
public:
    // Ctor
    SkyboxStage(
        Renderer& renderer,
        vk::RenderPass& render_pass,
        vk::DescriptorSet& camera_dset
    )
        : m_renderer_ptr{&renderer}
        , m_pipeline{
            vk::PipelineBuilder(render_pass)
                .set_vertex_stage(load_shader("shaders://fullscreen.vert.spv"))
                .set_fragment_stage(load_shader("shaders://skybox.frag.spv"))
                .set_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
                .set_depth_test(true)
                .set_depth_write(false)
                .set_depth_cmp(VK_COMPARE_OP_LESS_OR_EQUAL)
                .build(renderer.device())
        }
        , m_camera_dset_ptr{&camera_dset}
        , m_skybox_tex{
            load_texture("textures://skybox/space0.png"),
            load_texture("textures://skybox/space1.png"),
            load_texture("textures://skybox/space2.png"),
            load_texture("textures://skybox/space3.png"),
            load_texture("textures://skybox/space4.png"),
            load_texture("textures://skybox/space5.png"),
        }
        , m_skybox_dset{renderer.device().dset_allocator().allocate(
            *m_pipeline.dset_layout(1)
        )}
        , m_skybox_image(renderer.device(), m_skybox_tex[0]->vk_extent())
    {
        // Upload texture data to gpu image memory
        m_skybox_image.upload(
            (const void*[6]) {
                m_skybox_tex[0]->bytes,
                m_skybox_tex[1]->bytes,
                m_skybox_tex[2]->bytes,
                m_skybox_tex[3]->bytes,
                m_skybox_tex[4]->bytes,
                m_skybox_tex[5]->bytes
            }
        );
        // Update dset and upload data
        m_skybox_dset.update({m_skybox_image.info()});
    }

    void render(Scene& scene, vk::CommandBuffer& cmd_buffer) override {
        vk::cmd_bind_pipeline(cmd_buffer, m_pipeline);
        const auto swapchain_extent = m_renderer_ptr->swapchain().extent();
        vk::cmd_set_viewport(cmd_buffer, vk::create_viewport(swapchain_extent));
        vk::cmd_set_scissor(cmd_buffer, vk::create_scissor(swapchain_extent));

        vk::cmd_bind_dsets(
            cmd_buffer,
            // std::array{m_camera_dset_ptr, &m_earth_dset},
            std::array{m_camera_dset_ptr, &m_skybox_dset},
            m_pipeline.layout()
        );

        vk::cmd_draw(cmd_buffer, 4);
    }

private:
    Renderer* m_renderer_ptr;
    vk::Pipeline m_pipeline;
    vk::DescriptorSet* m_camera_dset_ptr;
    std::array<std::shared_ptr<TextureData>, 6> m_skybox_tex;
    vk::DescriptorSet m_skybox_dset;
    vk::CubeImage m_skybox_image;
};

} // namespace kzn