#pragma once

#include "core/log.hpp"
#include "core/window.hpp"
#include "ecs/entity.hpp"
#include "glm/fwd.hpp"
#include "graphics/passes/test_pass.hpp"
#include "graphics/render_image.hpp"
#include "graphics/renderer.hpp"
#include "vk/buffer.hpp"
#include "vk/cmd_buffer.hpp"
#include "vk/dset.hpp"
#include "vk/dset_layout.hpp"
#include "vk/functions.hpp"
#include "vk/pipeline.hpp"
#include "vk/render_pass.hpp"
#include "vk/uniform.hpp"

namespace kzn {

struct SpriteComponent {
    struct ShaderParams {
        glsl::Vec2 size{1.f, 1.f};
        glsl::Vec2 shift{0.f};
    } params;
};

class OffscreenPass : public Pass {

public:
    // Ctor
    OffscreenPass(RenderImage& render_image, Vec3& clear_color);
    // Dtor
    ~OffscreenPass() override = default;

    void render(vk::CommandBuffer& cmd_buffer) override;

private:
    Vec3& m_clear_color;
    RenderImage& m_render_image;
    vk::RenderPass m_offscreen_render_pass;
    vk::Framebuffer m_framebuffer;
    vk::DescriptorSet m_dset;
    vk::UniformBuffer m_tmp_ubo;
    vk::Pipeline m_pipeline;
};

} // namespace kzn