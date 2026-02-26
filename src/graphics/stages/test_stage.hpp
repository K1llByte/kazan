#pragma once

#include "graphics/renderer.hpp"
#include "graphics/stages/render_stage.hpp"
#include "graphics/texture.hpp"
#include "resources/resources.hpp"
#include "vk/dset_layout.hpp"
#include "vk/functions.hpp"
#include "vk/image.hpp"
#include "vk/pipeline.hpp"
#include "vk/render_pass.hpp"
#include <vulkan/vulkan_core.h>

namespace kzn {

class GpuPlanetStage : public RenderStage {
public:
    // Ctor
    GpuPlanetStage(Renderer& renderer, vk::RenderPass& render_pass, vk::DescriptorSet& camera_dset)
        : m_renderer_ptr{&renderer}
        , m_test_pipeline{
            renderer.device(),
            vk::PipelineStages{
                .vertex = load_shader("assets/shaders/planet/planet.vert.spv"),
                .fragment = load_shader("assets/shaders/planet/planet.frag.spv"),
            },
            vk::PipelineConfig(render_pass)
                .set_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
                .set_layout(vk::PipelineLayout{
                    .descriptor_sets = {
                        renderer.dset_layout_cache()
                            .layout({vk::uniform_binding(0)}),
                        renderer.dset_layout_cache()
                            .layout({vk::sampler_binding(0)})
                    }
                })
        }
        , m_camera_dset{camera_dset}
        , m_earth_tex_ptr{g_resources.find_or_load<Texture>("assets/textures/earth.jpg")}
        // , m_earth_tex_ptr{g_resources.find_or_load<Texture>("textures://earth.jpg")}
        , m_earth_dset{renderer.dset_allocator().allocate(
            renderer.dset_layout_cache().layout({
                vk::sampler_binding(0)
            })
        )}
        , m_earth_image(renderer.device(), m_earth_tex_ptr->extent())
    {
        // Upload texture data to gpu image memory
        m_earth_image.upload(m_earth_tex_ptr->data());
        // Update dset and upload data
        m_earth_dset.update({m_earth_image.info()});
    }

    void render(Scene& scene, vk::CommandBuffer& cmd_buffer) override {
        m_test_pipeline.bind(cmd_buffer);
        const auto swapchain_extent = m_renderer_ptr->swapchain().extent();
        vk::cmd_set_viewport(
            cmd_buffer, vk::create_viewport(swapchain_extent)
        );
        vk::cmd_set_scissor(
            cmd_buffer, vk::create_scissor(swapchain_extent)
        );

        // vk::cmd_bind_dset(
        //     m_camera_dset, cmd_buffer, m_test_pipeline.layout()
        // );

        vk::cmd_bind_dsets(
            std::array{
                Ref(m_camera_dset),
                Ref(m_earth_dset)
            },
            cmd_buffer,
            m_test_pipeline.layout()
        );
        
        vk::cmd_draw(cmd_buffer, 4);
    }

private:
    Renderer* m_renderer_ptr;
    vk::Pipeline m_test_pipeline;
    Ref<vk::DescriptorSet> m_camera_dset;
    std::shared_ptr<Texture> m_earth_tex_ptr;
    vk::DescriptorSet m_earth_dset;
    vk::Image m_earth_image;
};

} // namespace kzn