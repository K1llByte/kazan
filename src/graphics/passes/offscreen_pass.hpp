#pragma once

#include "graphics/passes/test_pass.hpp"
#include "graphics/render_image.hpp"
#include "vk/buffer.hpp"
#include "vk/cmd_buffer.hpp"
#include "vk/dset.hpp"
#include "vk/pipeline.hpp"
#include "vk/render_pass.hpp"
#include "vk/uniform.hpp"

namespace kzn {

struct SpriteComponent {
    struct ShaderParams {
        glsl::Mat3 mat;
    } params;

    struct RenderData {
        RenderData()
            : dset(Renderer::dset_allocator().allocate(
                  Renderer::dset_layout_cache().create_layout({
                      vk::uniform_binding(0),
                  })
              ))
            , ubo(Renderer::device(), sizeof(SpriteComponent::ShaderParams)) {
            dset.update({ubo.info()});
        }
        ~RenderData() { Log::debug("Destroyed RenderData"); }

        vk::DescriptorSet dset;
        vk::UniformBuffer ubo;
    } render_data;
};

class OffscreenPass : public Pass {

public:
    // Ctor
    OffscreenPass(RenderImage& render_image, Vec3& clear_color);
    // Dtor
    ~OffscreenPass() override { Log::debug("Destroyed OffscreenPass"); }

    void render(vk::CommandBuffer& cmd_buffer) override;

private:
    Vec3& m_clear_color;
    RenderImage& m_render_image;
    vk::RenderPass m_offscreen_render_pass;
    vk::Framebuffer m_framebuffer;
    vk::Pipeline m_pipeline;
};

} // namespace kzn