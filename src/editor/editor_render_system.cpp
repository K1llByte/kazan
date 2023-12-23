#include "editor_render_system.hpp"

#include "ecs/entity.hpp"
#include "events/event_manager.hpp"
#include "graphics/renderer.hpp"
#include "graphics/utils.hpp"
#include "math/transform.hpp"
#include "vk/functions.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

namespace kzn {

EditorRenderSystem::EditorRenderSystem(EditorWindow& window)
    : m_scoped_gfx_context(window)
    , m_renderer(
          GraphicsContext::device(),
          GraphicsContext::swapchain(),
          window
      )
    , m_main_render_pass(simple_pass(
          GraphicsContext::device(),
          GraphicsContext::swapchain().image_format()
      ))
    , m_framebuffers(create_swapchain_framebuffers(
          m_main_render_pass,
          GraphicsContext::swapchain()
      ))
    , m_window(window) {
    // Setupo event listeners
    EventManager::listen<SwapchainResize>(
        EventHandler(this, &EditorRenderSystem::on_swapchain_resize)
    );

    //////////////////
    //  ImGui Setup //
    //////////////////
    auto& instance = GraphicsContext::instance();
    auto& device = GraphicsContext::device();
    auto& swapchain = GraphicsContext::swapchain();

    VkDescriptorPoolSize pool_sizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    // Create ImGui descriptor pool
    VkDescriptorPool imgui_pool;
    auto res = vkCreateDescriptorPool(
        device.vk_device(), &pool_info, nullptr, &imgui_pool
    );
    VK_CHECK_MSG(res, "Error creating ImGui descriptor pool");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForVulkan(m_window.glfw_ptr(), true);

    // This initializes ImGui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = instance.vk_instance();
    init_info.PhysicalDevice = device.vk_physical_device();
    init_info.Device = device.vk_device();
    init_info.Queue = device.graphics_queue().vk_queue;
    init_info.DescriptorPool = imgui_pool;
    const uint32_t img_count = swapchain.images().size();
    init_info.MinImageCount = img_count;
    init_info.ImageCount = img_count;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info, m_main_render_pass.vk_render_pass());

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You
    // can also load multiple fonts and use ImGui::PushFont()/PopFont() to
    // select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if
    // you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please
    // handle those errors in your application (e.g. use an assertion, or
    // display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and
    // stored into a texture when calling
    // ImFontAtlas::Build()/GetTexDataAsXXXX(), which
    // ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a
    // string literal you need to write a double backslash \\ !
    io.Fonts->AddFontFromFileTTF("assets/fonts/ruda.bold.ttf", 16.0f);

    // TODO: Implement immediate_submit
    // vk::immediate_submit(device.graphics_queue(), [&](auto& cmd_buffer) {
    //     ImGui_ImplVulkan_CreateFontsTexture(cmd_buffer.vk_cmd_buffer());
    // });

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
}

EditorRenderSystem::~EditorRenderSystem() {
    GraphicsContext::device().wait_idle();
}

void EditorRenderSystem::update(float delta_time) {
    auto& swapchain = GraphicsContext::swapchain();

    const VkClearValue clear_color{ { { 0.01f, 0.01f, 0.01f, 1.0f } } };
    m_renderer.render_frame([&](auto& cmd_buffer) {
        // Begin Render Pass
        m_main_render_pass.begin(
            cmd_buffer,
            m_framebuffers[swapchain.current_index()],
            { clear_color }
        );

        // Draw Commands
        // Render GUI
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Draw commands
        m_window.update(delta_time);

        // Render dear imgui into screen
        ImGui::Render();

        // Submit draw commands
        ImGui_ImplVulkan_RenderDrawData(
            ImGui::GetDrawData(), cmd_buffer.vk_cmd_buffer()
        );

        // End Render Pass
        m_main_render_pass.end(cmd_buffer);
    });
}

void EditorRenderSystem::on_swapchain_resize(const SwapchainResize&) {
    m_framebuffers = create_swapchain_framebuffers(
        m_main_render_pass, GraphicsContext::swapchain()
    );
}

} // namespace kzn