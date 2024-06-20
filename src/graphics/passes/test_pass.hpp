#pragma once

#include "core/log.hpp"
#include "core/window.hpp"
#include "ecs/entity.hpp"
#include "graphics/renderer.hpp"
#include "vk/cmd_buffer.hpp"
#include "vk/functions.hpp"
#include "vk/pipeline.hpp"
#include "vk/render_pass.hpp"

namespace kzn {

struct Pass {
    virtual ~Pass() = default;
    virtual void render(vk::CommandBuffer& cmd_buffer) = 0;
};

class TestPass : public Pass {
public:
    // Ctor
    TestPass(Window& window);
    // Dtor
    ~TestPass() override = default;

    void render(vk::CommandBuffer& cmd_buffer) override;

    void on_swapchain_resize(const SwapchainResize&);

private:
    vk::RenderPass m_test_render_pass;
    std::vector<vk::Framebuffer> m_framebuffers;
    vk::Pipeline m_pipeline;
};

} // namespace kzn