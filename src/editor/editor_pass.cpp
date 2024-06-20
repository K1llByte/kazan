#include "editor_pass.hpp"
#include "graphics/renderer.hpp"
#include "graphics/utils.hpp"

#include "editor/editor_window.hpp"

namespace kzn {

EditorPass::EditorPass(EditorWindow& editor_window)
    : m_editor_window(editor_window)
    , m_framebuffers(create_swapchain_framebuffers(
          m_editor_window.render_context().render_pass(),
          Renderer::swapchain()
      )) {
    Log::debug("Created EditorPass");

    // Register listener to resize framebuffers
    EventManager::listen<SwapchainResize>(
        EventHandler(this, &EditorPass::on_swapchain_resize)
    );
}

EditorPass::~EditorPass() {
    // Renderer::device().wait_idle();

    // ImGui_ImplVulkan_Shutdown();
    // vkDestroyDescriptorPool(
    //     Renderer::device().vk_device(), m_imgui_pool, nullptr
    // );
}

void EditorPass::render(vk::CommandBuffer& cmd_buffer) {
    auto& swapchain = Renderer::swapchain();
    VkClearValue clear_color{{{0.01f, 0.01f, 0.01f, 1.0f}}};

    // Begin Render Pass
    auto& editor_render_pass = m_editor_window.render_context().render_pass();
    editor_render_pass.begin(
        cmd_buffer, m_framebuffers[swapchain.current_index()], {clear_color}
    );

    // Render GUI
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //////////////////// Draw commands ////////////////////
    m_editor_window.render();
    ///////////////////////////////////////////////////////

    // Render dear imgui into screen
    ImGui::Render();

    // Submit draw commands
    ImGui_ImplVulkan_RenderDrawData(
        ImGui::GetDrawData(), cmd_buffer.vk_cmd_buffer()
    );

    // End Render Pass
    editor_render_pass.end(cmd_buffer);
}

void EditorPass::on_swapchain_resize(const SwapchainResize&) {
    m_framebuffers = create_swapchain_framebuffers(
        m_editor_window.render_context().render_pass(), Renderer::swapchain()
    );
}

} // namespace kzn