#pragma once

#include "core/window.hpp"
#include "ecs/entity.hpp"
#include "glm/fwd.hpp"
#include "graphics/passes/test_pass.hpp"
#include "graphics/render_image.hpp"
#include "graphics/renderer.hpp"
#include "vk/cmd_buffer.hpp"
#include "vk/functions.hpp"
#include "vk/pipeline.hpp"
#include "vk/render_pass.hpp"

namespace kzn {

class OffscreenPass : public Pass {
public:
    // Ctor
    OffscreenPass(RenderImage& render_image, glm::vec3& clear_color);
    // Dtor
    ~OffscreenPass() = default;

    void render(vk::CommandBuffer& cmd_buffer) override;

private:
    glm::vec3& m_clear_color;
    RenderImage& m_render_image;
    vk::RenderPass m_offscreen_render_pass;
    vk::Framebuffer m_framebuffer;
    vk::Pipeline m_pipeline;
};

} // namespace kzn