#include "gui/ui.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

// Debug
#include <iostream>

namespace kzn
{
    Interface::Interface(Renderer* _renderer, Window& _window)
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
        ImGui_ImplVulkan_Init(&init_info, renderer->main_render_pass()->vk_render_pass());

        // Implementation of single time commands
        {
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

        //
        //auto cmd_buffer = device.begin_single_time_commands();
        //ImGui_ImplVulkan_CreateFontsTexture(cmd_buffer);
        //device.end_single_time_commands(cmd_buffer);

        //clear font textures from cpu data
        ImGui_ImplVulkan_DestroyFontUploadObjects();

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
    }


    Interface::~Interface() {
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

            ImGui::ShowDemoWindow();
            ImGui::Begin("Demo window");
            ImGui::Button("Hello!");
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
}