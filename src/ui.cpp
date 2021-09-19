#include "ui.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

namespace kzn
{

Interface::Interface(
    Window& window,
    Instance& instance,
    Device& device,
    Renderer& renderer)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForVulkan(window.window(), true);

    // This initializes imgui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = instance._instance;
    init_info.PhysicalDevice = device._physical_device;
    init_info.Device = device._device;
    init_info.Queue = device._graphics_queue;
    init_info.DescriptorPool = nullptr; //imguiPool;
    const uint32_t img_count = renderer._swap_chain->image_count();
    init_info.MinImageCount = img_count;
    init_info.ImageCount = img_count;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info, renderer._swap_chain->render_pass());
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    initialized = true;
}


Interface::~Interface()
{
    if(initialized)
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}

    
void Interface::render()
{
    if(initialized)
    {
        // Render your GUI
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Demo window");
        ImGui::Button("Hello!");
        ImGui::End();
        
        // Render dear imgui into screen
        ImGui::Render();
    }
}

}
