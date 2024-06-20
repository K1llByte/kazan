#pragma once

#include "editor/editor_window.hpp"
#include "graphics/passes/test_pass.hpp"
#include "graphics/render_image.hpp"

namespace kzn {

class EditorPass : public Pass {
public:
    // Ctor
    EditorPass(EditorWindow& editor_window);
    // Dtor
    ~EditorPass();

    void render(vk::CommandBuffer& cmd_buffer) override;

    void on_swapchain_resize(const SwapchainResize&);

private:
    EditorWindow& m_editor_window;
    // vk::RenderPass m_editor_render_pass;
    std::vector<vk::Framebuffer> m_framebuffers;
    // VkDescriptorPool m_imgui_pool = VK_NULL_HANDLE;
};

} // namespace kzn