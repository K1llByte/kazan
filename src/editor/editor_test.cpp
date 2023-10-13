#include "kazan.hpp"

#include <cmath>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

using namespace kzn;


vk::Pipeline triangle_pipeline(vk::RenderPass& render_pass) {
    return vk::Pipeline(
        render_pass.device(),
        vk::PipelineStages{
            .vertex = "assets/shaders/triangle/triangle.vert.spv",
            .fragment = "assets/shaders/triangle/triangle.frag.spv",
        },
        vk::PipelineConfig(render_pass)
    );
}

void set_theme() {
    ImGuiStyle* style = &ImGui::GetStyle();

    // SRGB Palette
    // // const auto AQUA         = ImVec4(0.408f, 0.616f, 0.416f, 1.f); // 689d6a
    const auto DEBUG           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f); // ff0000
    const auto DARK_GREY       = ImVec4(0.004753f, 0.004006f, 0.006017f, 1.00f); // 0f0d12
    const auto DARK_GREY_HOVER = ImVec4(0.046964f, 0.043233f, 0.068384f, 1.00f);
    const auto GREY            = ImVec4(0.010022f, 0.008540f, 0.013411f, 1.00f); // 1a171f
    const auto GREY_HOVER      = ImVec4(0.019606f, 0.015325f, 0.027211f, 1.00f); // 8f8f94
    const auto GREY_ACTIVE     = ImVec4(0.603827f, 0.603827f, 0.655930f, 0.31f);

    style->WindowPadding     = ImVec2(15, 15);
    style->WindowRounding    = 5.0f;
    style->FramePadding      = ImVec2(5, 5);
    style->FrameRounding     = 4.0f;
    style->ItemSpacing       = ImVec2(12, 8);
    style->ItemInnerSpacing  = ImVec2(8, 6);
    style->IndentSpacing     = 25.0f;
    style->ScrollbarSize     = 15.0f;
    style->ScrollbarRounding = 9.0f;
    style->GrabMinSize       = 5.0f;
    style->GrabRounding      = 3.0f;

    style->Colors[ImGuiCol_Text]                  = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    style->Colors[ImGuiCol_TextDisabled]          = DARK_GREY_HOVER;
    style->Colors[ImGuiCol_WindowBg]              = DARK_GREY;
    style->Colors[ImGuiCol_ChildBg]               = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_PopupBg]               = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_Border]                = GREY; // ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
    style->Colors[ImGuiCol_BorderShadow]          = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
    style->Colors[ImGuiCol_FrameBg]               = GREY;
    style->Colors[ImGuiCol_FrameBgHovered]        = DARK_GREY_HOVER;
    style->Colors[ImGuiCol_FrameBgActive]         = GREY_HOVER;
    style->Colors[ImGuiCol_TitleBg]               = GREY;
    style->Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
    style->Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_MenuBarBg]             = GREY;
    style->Colors[ImGuiCol_ScrollbarBg]           = GREY;
    style->Colors[ImGuiCol_ScrollbarGrab]         = GREY_HOVER; //ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_ScrollbarGrabHovered]  = DARK_GREY_HOVER;
    style->Colors[ImGuiCol_ScrollbarGrabActive]   = GREY_ACTIVE; // DARK_GREY;
    style->Colors[ImGuiCol_CheckMark]             = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrab]            = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrabActive]      = DARK_GREY;
    style->Colors[ImGuiCol_Button]                = GREY;
    style->Colors[ImGuiCol_ButtonHovered]         = DARK_GREY_HOVER;
    style->Colors[ImGuiCol_ButtonActive]          = GREY_HOVER;
    style->Colors[ImGuiCol_Header]                = GREY;
    style->Colors[ImGuiCol_HeaderHovered]         = GREY_HOVER;
    style->Colors[ImGuiCol_HeaderActive]          = DARK_GREY;
    style->Colors[ImGuiCol_Separator]             = DARK_GREY;
    style->Colors[ImGuiCol_SeparatorHovered]      = DARK_GREY_HOVER;
    style->Colors[ImGuiCol_SeparatorActive]       = GREY_HOVER;

    style->Colors[ImGuiCol_Tab]                   = DARK_GREY;
    style->Colors[ImGuiCol_TabHovered]            = DARK_GREY_HOVER;
    style->Colors[ImGuiCol_TabActive]             = GREY;
    style->Colors[ImGuiCol_TabUnfocused]          = DARK_GREY;
    style->Colors[ImGuiCol_TabUnfocusedActive]    = GREY;

    style->Colors[ImGuiCol_DockingPreview]        = DARK_GREY; //_HOVER;
    style->Colors[ImGuiCol_DockingEmptyBg]        = DARK_GREY; //_HOVER;

    style->Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style->Colors[ImGuiCol_ResizeGripHovered]     = GREY_HOVER;
    style->Colors[ImGuiCol_ResizeGripActive]      = DARK_GREY;
    // style->Colors[ImGuiCol_CloseButton]        = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
    // style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
    // style->Colors[ImGuiCol_CloseButtonActive]  = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
    style->Colors[ImGuiCol_PlotLines]             = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
    // style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}

void draw_ui(vk::CommandBuffer& cmd_buffer) {
    // Render your GUI
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport();
    ImGui::ShowDemoWindow();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport");
    ImGui::PopStyleVar();
    ImGui::End();

    // Render dear imgui into screen
    ImGui::Render();

    // Submit draw commands
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd_buffer.vk_cmd_buffer());
}

struct Transform2DComponent {
    int a = 1;
};

int main() try {
    Entity player = Registry::create();
    player.add_component<Transform2DComponent>(1);
    Registry::destroy(player);

    auto window = Window("Kazan Engine", 1000, 800);

    auto instance = vk::Instance({
        .extensions = window.required_extensions(),
        .with_validation = true,
    });
    auto surface = window.create_surface(instance);
    auto device = vk::Device(instance, {
        .extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME },
        .surface = surface.vk_surface()
    });
    auto swapchain = vk::Swapchain(device, surface, window.extent());

    auto render_pass = simple_pass(device, swapchain.image_format());
    auto framebuffers = create_swapchain_framebuffers(render_pass, swapchain);

    auto pipeline = triangle_pipeline(render_pass);

    auto renderer = Renderer(device, swapchain, window);

    renderer.on_swapchain_resize([&] {
        framebuffers = create_swapchain_framebuffers(render_pass, swapchain);
    });

    //////////////////
    //  ImGui Setup //
    //////////////////

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
    auto res = vkCreateDescriptorPool(device.vk_device(), &pool_info, nullptr, &imgui_pool);
    VK_CHECK_MSG(res, "Error creating ImGui descriptor pool");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForVulkan(window.glfw_ptr(), true);
    
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

    ImGui_ImplVulkan_Init(&init_info, render_pass.vk_render_pass()); 

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    io.Fonts->AddFontFromFileTTF("assets/fonts/ruda.bold.ttf", 16.0f);

    // TODO: Implement immediate_submit
    vk::immediate_submit(device.graphics_queue(), [&](auto& cmd_buffer) {
        ImGui_ImplVulkan_CreateFontsTexture(cmd_buffer.vk_cmd_buffer());
    });

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    set_theme();

    // Todo render texture

    //////////////////
    //  Render Loop //
    //////////////////

    size_t frame_counter = 0;
    while(!window.is_closed()) {
        window.poll_events();

        // Render
        renderer.render_frame([&](auto& cmd_buffer) {
            // Render pass
            render_pass.begin(
                cmd_buffer,
                framebuffers[swapchain.current_index()],
                { VkClearValue{{{0.01f, 0.01f, 0.01f, 1.0f}}} });

            // vk::cmd_set_viewport(cmd_buffer, vk::create_viewport(swapchain.extent()));
            // vk::cmd_set_scissor(cmd_buffer, vk::create_scissor(swapchain.extent()));
            
            // pipeline.bind(cmd_buffer);

            // vk::cmd_draw(cmd_buffer, 3);
            draw_ui(cmd_buffer);
            render_pass.end(cmd_buffer);
        });

        // Increment frame counter
        frame_counter += 1;
    }

    device.wait_idle();

    vkDestroyDescriptorPool(device.vk_device(), imgui_pool, nullptr);
    ImGui_ImplVulkan_Shutdown();
}
catch(vk::ResultError re) {
    Log::error("{}", re.message());
}