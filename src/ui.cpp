#include "ui.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

// Debug
#include <iostream>

namespace kzn
{

Interface::Interface(
    Window& window,
    Instance& instance,
    Device& device,
    Renderer& renderer)
{
    _device = device.device();
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
    pool_info.poolSizeCount = pool_sizes.size();
    pool_info.pPoolSizes = pool_sizes.data();

    if(vkCreateDescriptorPool(_device, &pool_info, nullptr, &imgui_pool) != VK_SUCCESS)
    {
        throw std::runtime_error("error creating imgui descriptor pool");
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // ImGuiIO &io = ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForVulkan(window.window(), true);

    // This initializes imgui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = instance._instance;
    init_info.PhysicalDevice = device._physical_device;
    init_info.Device = _device;
    init_info.Queue = device._graphics_queue;
    init_info.DescriptorPool = imgui_pool;
    const uint32_t img_count = renderer._swap_chain->image_count();
    init_info.MinImageCount = img_count;
    init_info.ImageCount = img_count;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    std::cout << "ImGui_ImplVulkan_Init called\n";
    ImGui_ImplVulkan_Init(&init_info, renderer._swap_chain->render_pass());

    auto cmd_buffer = device.begin_single_time_commands();
    ImGui_ImplVulkan_CreateFontsTexture(cmd_buffer);
    device.end_single_time_commands(cmd_buffer);

    //clear font textures from cpu data
    ImGui_ImplVulkan_DestroyFontUploadObjects();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    initialized = true;
}


void Interface::cleanup()
{
    if(initialized)
    {
        std::cout << "Destroyed things\n";
        vkDestroyDescriptorPool(_device, imgui_pool, nullptr);
		ImGui_ImplVulkan_Shutdown();

        // ImGui_ImplVulkan_Shutdown();
        // ImGui_ImplGlfw_Shutdown();
        // ImGui::DestroyContext();
    }
}

    
void Interface::render()
{
    if(initialized)
    {
        // Render your GUI
        std::cout << "ImGui_ImplVulkan_NewFrame\n";
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();
        // ImGui::Begin("Demo window");
        // ImGui::Button("Hello!");
        // ImGui::End();
        
        // Render dear imgui into screen
        ImGui::Render();
    }
}

void Interface::draw(VkCommandBuffer cmd_buffer)
{
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd_buffer);
}

}