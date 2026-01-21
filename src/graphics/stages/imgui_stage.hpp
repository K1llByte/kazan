#pragma once

#include "core/assert.hpp"
#include "graphics/renderer.hpp"
#include "render_stage.hpp"
#include "vk/render_pass.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <vulkan/vulkan_core.h>

#include <vector>

namespace kzn {

class ImGuiStage : public RenderStage {
public:
    // Ctor
    ImGuiStage(vk::RenderPass& render_pass) {
        KZN_ASSERT(is_editor_initialized());
        // Naive pool size
        std::vector<VkDescriptorPoolSize> pool_sizes{
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
        pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
        pool_info.pPoolSizes = pool_sizes.data();

        // Create ImGui descriptor pool
        auto res = vkCreateDescriptorPool(
            Renderer::device(), &pool_info, nullptr, &m_imgui_pool
        );
        VK_CHECK_MSG(res, "Error creating ImGui descriptor pool");

        // // Setup Dear ImGui context
        // IMGUI_CHECKVERSION();
        // ImGui::CreateContext();
        // ImGuiIO& io = ImGui::GetIO();
        // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        // // Setup Platform/Renderer bindings
        // ImGui_ImplGlfw_InitForVulkan(window.glfw_ptr(), true);

        // This initializes imgui for Vulkan
        ImGui_ImplVulkan_InitInfo init_info{};
        init_info.Instance = Renderer::instance();
        init_info.PhysicalDevice = Renderer::device();
        init_info.Device = Renderer::device();
        init_info.Queue = Renderer::device().graphics_queue().vk_queue;
        init_info.DescriptorPool = m_imgui_pool;
        const uint32_t img_count =
            static_cast<uint32_t>(Renderer::swapchain().images().size());
        init_info.MinImageCount = img_count;
        init_info.ImageCount = img_count;
        init_info.PipelineInfoMain.RenderPass = render_pass;
        init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&init_info);
    }
    // Copy
    ImGuiStage(const ImGuiStage&) = delete;
    ImGuiStage& operator=(const ImGuiStage&) = delete;
    // Move
    ImGuiStage(ImGuiStage&&) = delete;
    ImGuiStage& operator=(ImGuiStage&&) = delete;
    // Dtor
    ~ImGuiStage() {
        KZN_ASSERT(is_editor_initialized());
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        vkDestroyDescriptorPool(Renderer::device(), m_imgui_pool, nullptr);
    }

    void pre_render() override {
        // Render dear imgui into screen
        ImGui::Render();
    }

    void render(vk::CommandBuffer& cmd_buffer) override {
        KZN_ASSERT(is_editor_initialized());

        // Submit draw commands
        ImGui_ImplVulkan_RenderDrawData(
            ImGui::GetDrawData(), cmd_buffer.vk_cmd_buffer()
        );
    }

    void post_render() override {}

private:
    [[nodiscard]]
    bool is_editor_initialized() const {
        return ImGui::GetCurrentContext() != nullptr;
    }
    [[nodiscard]]
    bool has_editor_submitted_data() const {
        return false;
    }

private:
    VkDescriptorPool m_imgui_pool = VK_NULL_HANDLE;
};

} // namespace kzn