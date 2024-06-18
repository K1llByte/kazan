#include "editor_pass.hpp"
#include "graphics/utils.hpp"

#include "editor/editor_window.hpp"

namespace kzn {

EditorPass::EditorPass(EditorWindow& editor_window)
    : m_editor_window(editor_window)
    , m_editor_render_pass(
          simple_pass(Renderer::device(), Renderer::swapchain().image_format())
      )
    , m_framebuffers(create_swapchain_framebuffers(
          m_editor_render_pass,
          Renderer::swapchain()
      )) {
    VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
    };

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    auto& device = Renderer::device();

    // Create ImGui descriptor pool
    auto res = vkCreateDescriptorPool(
        device.vk_device(), &pool_info, nullptr, &m_imgui_pool
    );
    VK_CHECK_MSG(res, "Error creating ImGui descriptor pool");

    ImGui_ImplGlfw_InitForVulkan(editor_window.root_window().glfw_ptr(), true);

    // This initializes ImGui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = Renderer::instance().vk_instance();
    init_info.PhysicalDevice = device.vk_physical_device();
    init_info.Device = device.vk_device();
    init_info.Queue = device.graphics_queue().vk_queue;
    init_info.DescriptorPool = m_imgui_pool;
    init_info.RenderPass = m_editor_render_pass.vk_render_pass();

    const uint32_t img_count = Renderer::swapchain().images().size();
    init_info.MinImageCount = img_count;
    init_info.ImageCount = img_count;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info);

    // Register listener to resize framebuffers
    EventManager::listen<SwapchainResize>(
        EventHandler(this, &EditorPass::on_swapchain_resize)
    );
}

EditorPass::~EditorPass() {
    Renderer::device().wait_idle();

    ImGui_ImplVulkan_Shutdown();
    vkDestroyDescriptorPool(
        Renderer::device().vk_device(), m_imgui_pool, nullptr
    );
}

void EditorPass::render(vk::CommandBuffer& cmd_buffer) {
    auto& swapchain = Renderer::swapchain();
    VkClearValue clear_color{{{0.01f, 0.01f, 0.01f, 1.0f}}};

    // Begin Render Pass
    m_editor_render_pass.begin(
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
    m_editor_render_pass.end(cmd_buffer);
}

void EditorPass::on_swapchain_resize(const SwapchainResize&) {
    m_framebuffers = create_swapchain_framebuffers(
        m_editor_render_pass, Renderer::swapchain()
    );
}

} // namespace kzn