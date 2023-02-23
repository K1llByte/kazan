#include "gui/ui.hpp"

// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_vulkan.h"

// float to_srgb(float l) {
//     return (l <= 0.0031308f)
//         ? l * 12.92f
//         : 1.055f * pow(l, 1.0f/2.4f) - 0.055f;
// }

// float to_linear(float l) {
//     return (l <= 0.04045f)
//         ? l / 12.92f
//         : pow((l + 0.055f) / 1.055f, 2.4f);
// }

ImVec4 to_srgb(ImVec4 rgba) {
    rgba.x /= 4.5f; // to_srgb(rgba.x);
    rgba.y /= 4.5f; // to_srgb(rgba.y);
    rgba.z /= 4.5f; // to_srgb(rgba.z);
    return rgba;
}

namespace kzn {
    Interface::Interface(OffscreenRenderer* _renderer, Window& _window)
        : renderer(_renderer)
    {
        std::vector<VkDescriptorPoolSize> pool_sizes{
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
        pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
        pool_info.pPoolSizes = pool_sizes.data();

        {
            using namespace kzn::vk;
            auto res = vkCreateDescriptorPool(Context::device().vk_device(), &pool_info, nullptr, &imgui_pool);
            VK_CHECK_MSG(res, "error creating imgui descriptor pool");
        }

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForVulkan(_window.glfw_ptr(), true);

        // This initializes imgui for Vulkan
        ImGui_ImplVulkan_InitInfo init_info{};
        init_info.Instance = Context::instance().vk_instance();
        init_info.PhysicalDevice = Context::device().vk_physical_device();
        init_info.Device = Context::device().vk_device();
        init_info.Queue = Context::device().vk_graphics_queue();
        init_info.DescriptorPool = imgui_pool;
        const uint32_t img_count = static_cast<uint32_t>(Context::swapchain().num_images());
        init_info.MinImageCount = img_count;
        init_info.ImageCount = img_count;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        // TODO: Get main render pass
        // ImGui_ImplVulkan_Init(&init_info, renderer->main_render_pass()->vk_render_pass());
        
        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
        // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
        // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
        // - Read 'docs/FONTS.md' for more instructions and details.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        io.Fonts->AddFontFromFileTTF("assets/fonts/ruda.bold.ttf", 16.0f);

        // Implementation of single time commands
        {
            // Context::device().immediate_submit([&](){
            //     ImGui_ImplVulkan_CreateFontsTexture(cmd_buffer.vk_command_buffer());
            // });
            auto cmd_buffer = renderer->get_cmd_pool().allocate();
            cmd_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
            ImGui_ImplVulkan_CreateFontsTexture(cmd_buffer.vk_command_buffer());
            cmd_buffer.end();
            // submit
            Context::device().graphics_queue_submit(cmd_buffer);
            // wait queue idle
            vkQueueWaitIdle(Context::device().vk_graphics_queue());
            // cmd buffer will be freed when it's out of scope
        }

        //clear font textures from cpu data
        ImGui_ImplVulkan_DestroyFontUploadObjects();

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        this->set_theme();

        // Create image view for each render image and sampler
        // And finally create all imgui texture ids
        const auto& render_images = renderer->get_render_images();
        render_tex_ids.reserve(render_images.size());
        render_image_views.reserve(render_images.size());
        render_image_sampler = vk::create_sampler(Context::device());
        for(size_t i = 0; i < render_images.size(); ++i) {
            // Create image view
            render_image_views.push_back(
                vk::create_image_view(
                    Context::device(),
                    render_images[i],
                    VK_FORMAT_B8G8R8A8_SRGB,
                    VK_IMAGE_ASPECT_COLOR_BIT
                )
            );
            // Get ImTextureID
            render_tex_ids.push_back(
                ImGui_ImplVulkan_AddTexture(
                    render_image_sampler,
                    render_image_views[i],
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
            );
        }
    }


    Interface::~Interface() {
        for(const auto& img_view : render_image_views) {
            vk::destroy_image_view(
                Context::device(),
                img_view
            );
        }
        vk::destroy_sampler(Context::device(), render_image_sampler);
        
        vkDestroyDescriptorPool(Context::device().vk_device(), imgui_pool, nullptr);
        ImGui_ImplVulkan_Shutdown();

        // ImGui_ImplVulkan_Shutdown();
        // ImGui_ImplGlfw_Shutdown();
        // ImGui::DestroyContext();
    }


    void Interface::toggle()
    {
        render_ui = !render_ui;
    }


    void Interface::setup()
    {
        if(render_ui)
        {
            // Render your GUI
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::DockSpaceOverViewport();
            ImGui::ShowDemoWindow();
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("Viewport");
            ImGui::PopStyleVar();
            

            // // ImGui::Button("Hello!");
            // ImGui::PopStyleVar();
            auto [w,h] = ImGui::GetContentRegionAvail();
            
            ImGui::Image(render_tex_ids[Context::swapchain().current_index()], ImVec2(w,h));
            ImGui::End();
            
            // Render dear imgui into screen
            ImGui::Render();
        }
    }

    void Interface::draw(vk::CommandBuffer& cmd_buffer)
    {
        if(render_ui)
        {
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd_buffer.vk_command_buffer());
        }
    }


    void Interface::set_theme()
    {
        ImGuiStyle* style = &ImGui::GetStyle();

        // UNORM Palette
        // const auto AQUA         = ImVec4(0.408f, 0.616f, 0.416f, 1.f); // 689d6a
        const auto DARK_GREY       = to_srgb(ImVec4(0.06f, 0.05f, 0.07f, 1.00f)); // 0f0d12
        const auto DARK_GREY_HOVER = to_srgb(ImVec4(0.24f, 0.23f, 0.29f, 1.00f));
        const auto GREY            = to_srgb(ImVec4(0.10f, 0.09f, 0.12f, 1.00f)); // 1a171f
        // const auto GREY_HOVER      = ImVec4(0.56f, 0.56f, 0.58f, 1.00f); // 8f8f94
        const auto GREY_HOVER      = ImVec4(0.15f, 0.13f, 0.18f, 1.00f); // 8f8f94
        const auto GREY_ACTIVE     = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        const auto DEBUG           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f); // ff0000

        // SRGB Palette
        // // const auto AQUA         = ImVec4(0.408f, 0.616f, 0.416f, 1.f); // 689d6a
        // const auto DEBUG           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f); // ff0000
        // const auto DARK_GREY       = ImVec4(0.004753f, 0.004006f, 0.006017f, 1.00f); // 0f0d12
        // const auto DARK_GREY_HOVER = ImVec4(0.046964f, 0.043233f, 0.068384f, 1.00f);
        // const auto GREY            = ImVec4(0.010022f, 0.008540f, 0.013411f, 1.00f); // 1a171f
        // const auto GREY_HOVER      = ImVec4(0.019606f, 0.015325f, 0.027211f, 1.00f); // 8f8f94
        // const auto GREY_ACTIVE     = ImVec4(0.603827f, 0.603827f, 0.655930f, 0.31f);

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
}