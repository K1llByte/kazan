#include "editor_window.hpp"
#include "backends/imgui_impl_vulkan.h"
#include "core/log.hpp"
#include "core/window.hpp"
#include "ecs/entity.hpp"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "events/event_manager.hpp"
#include "fmt/format.h"
#include "graphics/passes/offscreen_pass.hpp"
#include "graphics/renderer.hpp"
#include "imgui.h"
#include "math/transform.hpp"
#include <limits>
#include <optional>

namespace kzn {

void EntityListPanel::render() {
    ImGui::Begin("Entities List");
    const ImVec2 button_size = ImVec2(-FLT_MIN, 0.0f);
    if (ImGui::Button("Create Entity", button_size)) {
        Registry::create();
    }

    ImGui::Spacing();

    static entt::entity selected_entity = entt::null;
    for (auto entity : Registry::registry.view<entt::entity>()) {
        auto entity_name = fmt::format("Entity {}", std::uint32_t(entity));

        if (ImGui::Selectable(entity_name.c_str(), selected_entity == entity)) {
            selected_entity = entity;
            EventManager::send(EntitySelectedEvent{
                .entity = Entity(entity),
            });
        }
    }

    ImGui::End();
}

void render_component(Transform2DComponent& component) {
    if (ImGui::CollapsingHeader(
            "Transform Component", ImGuiTreeNodeFlags_DefaultOpen
        )) {
        ImGui::DragFloat2(
            "Position",
            &component.position.x,
            0.01f,
            -std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max()
        );
        ImGui::DragFloat(
            "Rotation",
            &component.rotation,
            0.01f,
            -std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max()
        );
        ImGui::DragFloat2(
            "Scale",
            &component.scale.x,
            0.01f,
            -std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max()
        );
    }
}

void render_component(SpriteComponent& component) {
    if (ImGui::CollapsingHeader(
            "Sprite Component", ImGuiTreeNodeFlags_DefaultOpen
        )) {

        ImGui::DragFloat2(
            "Shift", &component.params.shift.x, 0.01f, -1.0f, 1.0f
        );
        ImGui::DragFloat2("Size", &component.params.size.x, 0.01f, 0.0f, 10.0f);
    }
}

void InspectorPanel::render() {
    ImGui::ShowDemoWindow();
    ImGui::Begin("Inspector");

    ImGui::ColorEdit3("Clear color", &m_tmp.x);
    ImGui::Spacing();
    // ImGui::DragFloat3("Clear color", &m_tmp.x, 0.01f, 0.0f, 1.0f);

    if (m_entity.has_value()) {
        const auto entity_name =
            fmt::format("Entity {}", std::uint32_t(m_entity->raw()));
        ImGui::Text(entity_name.c_str());
        ImGui::Spacing();
        auto transform2d_comp =
            Registry::registry.try_get<Transform2DComponent>(
                m_entity.value().raw()
            );
        if (transform2d_comp != nullptr) {
            render_component(*transform2d_comp);
        }
        auto sprite_comp =
            Registry::registry.try_get<SpriteComponent>(m_entity.value().raw());
        if (sprite_comp != nullptr) {
            render_component(*sprite_comp);
        }
    }
    ImGui::End();
}

ViewportPanel::ViewportPanel()
    : m_render_image(
          Renderer::device(),
          {1280, 720},
          Renderer::swapchain().image_format()
      ) {
    m_render_image_tex_id = ImGui_ImplVulkan_AddTexture(
        m_render_image.sampler(),
        m_render_image.image_view(),
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );
}

ViewportPanel::~ViewportPanel() {
    ImGui_ImplVulkan_RemoveTexture(
        static_cast<VkDescriptorSet>(m_render_image_tex_id)
    );
}

void ViewportPanel::render() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport");

    const auto region = ImGui::GetContentRegionAvail();
    ImGui::Image(m_render_image_tex_id, region);

    ImGui::PopStyleVar();
    ImGui::End();
}

EditorRenderContext::EditorRenderContext(Window& window)
    : m_editor_render_pass(
          simple_pass(Renderer::device(), Renderer::swapchain().image_format())
      ) {
    // Create base imgui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForVulkan(window.glfw_ptr(), true);

    // Initialize ImGui descriptor pool
    const VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1024},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1024},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1024},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1024},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1024},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1024},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1024},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1024},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1024},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1024},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1024}
    };

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1024;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    auto& device = Renderer::device();
    auto res =
        vkCreateDescriptorPool(device, &pool_info, nullptr, &m_imgui_pool);
    VK_CHECK_MSG(res, "Error creating ImGui descriptor pool");

    // This initializes ImGui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = Renderer::instance();
    init_info.PhysicalDevice = device;
    init_info.Device = device;
    init_info.Queue = device.graphics_queue().vk_queue;
    init_info.DescriptorPool = m_imgui_pool;
    init_info.RenderPass = m_editor_render_pass;

    const uint32_t img_count = Renderer::swapchain().images().size();
    init_info.MinImageCount = img_count;
    init_info.ImageCount = img_count;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info);
}

EditorRenderContext::~EditorRenderContext() {
    // Need to wait for the device to finish any work before shutting down ImGui
    // context.
    Renderer::device().wait_idle();

    ImGui_ImplVulkan_Shutdown();
    vkDestroyDescriptorPool(Renderer::device(), m_imgui_pool, nullptr);
}

EditorWindow::EditorWindow(Window& window)
    : m_window(window)
    , m_render_context(window) {

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

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.Fonts->AddFontFromFileTTF("assets/fonts/ruda.bold.ttf", 16.0f);

    // TODO: Implement immediate_submit
    // vk::immediate_submit(device.graphics_queue(), [&](auto& cmd_buffer) {
    //     ImGui_ImplVulkan_CreateFontsTexture(cmd_buffer.vk_cmd_buffer());
    // });

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    set_theme();
}

void EditorWindow::render() {

    // Draw commands
    ImGui::DockSpaceOverViewport();
    // ImGui::ShowDemoWindow();

    for (const auto& panel_ptr : m_panels) {
        panel_ptr->render();
    }
}

void EditorWindow::set_theme() {
    ImGuiStyle* style = &ImGui::GetStyle();

    // SRGB Palette
    // // const auto AQUA         = ImVec4(0.408f, 0.616f, 0.416f, 1.f); //
    // 689d6a
    // const auto DEBUG = ImVec4(1.00f, 0.00f, 0.00f, 1.00f); // ff0000
    const auto DARK_GREY =
        ImVec4(0.004753f, 0.004006f, 0.006017f, 1.00f); // 0f0d12
    const auto DARK_GREY_HOVER = ImVec4(0.046964f, 0.043233f, 0.068384f, 1.00f);
    // const auto GREY = ImVec4(0.010022f, 0.008540f, 1.0f, 1.00f); // 1a171f
    const auto GREY = ImVec4(0.010022f, 0.008540f, 0.013411f, 1.00f); //
    // 1a171f
    const auto GREY_HOVER =
        ImVec4(0.019606f, 0.015325f, 0.027211f, 1.00f); // 8f8f94
    const auto GREY_ACTIVE = ImVec4(0.603827f, 0.603827f, 0.655930f, 0.31f);

    style->WindowPadding = ImVec2(15, 15);
    style->WindowRounding = 5.0f;
    style->FramePadding = ImVec2(5, 5);
    style->FrameRounding = 4.0f;
    style->ItemSpacing = ImVec2(12, 8);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = 15.0f;
    style->ScrollbarRounding = 9.0f;
    style->GrabMinSize = 5.0f;
    style->GrabRounding = 3.0f;

    style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    style->Colors[ImGuiCol_TextDisabled] = DARK_GREY_HOVER;
    style->Colors[ImGuiCol_WindowBg] = DARK_GREY;
    style->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_Border] =
        GREY; // ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
    style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
    style->Colors[ImGuiCol_FrameBg] = GREY;
    style->Colors[ImGuiCol_FrameBgHovered] = DARK_GREY_HOVER;
    style->Colors[ImGuiCol_FrameBgActive] = GREY_HOVER;
    style->Colors[ImGuiCol_TitleBg] = GREY;
    style->Colors[ImGuiCol_TitleBgCollapsed] =
        ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_MenuBarBg] = GREY;
    style->Colors[ImGuiCol_ScrollbarBg] = GREY;
    style->Colors[ImGuiCol_ScrollbarGrab] =
        GREY_HOVER; // ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = DARK_GREY_HOVER;
    style->Colors[ImGuiCol_ScrollbarGrabActive] = GREY_ACTIVE; // DARK_GREY;
    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrabActive] = DARK_GREY;
    style->Colors[ImGuiCol_Button] = GREY;
    style->Colors[ImGuiCol_ButtonHovered] = DARK_GREY_HOVER;
    style->Colors[ImGuiCol_ButtonActive] = GREY_HOVER;
    style->Colors[ImGuiCol_Header] = GREY;
    style->Colors[ImGuiCol_HeaderHovered] = GREY_HOVER;
    style->Colors[ImGuiCol_HeaderActive] = DARK_GREY;
    style->Colors[ImGuiCol_Separator] = DARK_GREY;
    style->Colors[ImGuiCol_SeparatorHovered] = DARK_GREY_HOVER;
    style->Colors[ImGuiCol_SeparatorActive] = GREY_HOVER;

    style->Colors[ImGuiCol_Tab] = DARK_GREY;
    style->Colors[ImGuiCol_TabHovered] = DARK_GREY_HOVER;
    style->Colors[ImGuiCol_TabActive] = GREY;
    style->Colors[ImGuiCol_TabUnfocused] = DARK_GREY;
    style->Colors[ImGuiCol_TabUnfocusedActive] = GREY;

    style->Colors[ImGuiCol_DockingPreview] = DARK_GREY; //_HOVER;
    style->Colors[ImGuiCol_DockingEmptyBg] = DARK_GREY; //_HOVER;

    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style->Colors[ImGuiCol_ResizeGripHovered] = GREY_HOVER;
    style->Colors[ImGuiCol_ResizeGripActive] = DARK_GREY;
    // style->Colors[ImGuiCol_CloseButton]        = ImVec4(0.40f, 0.39f,
    // 0.38f, 0.16f); style->Colors[ImGuiCol_CloseButtonHovered] =
    // ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
    // style->Colors[ImGuiCol_CloseButtonActive]  = ImVec4(0.40f, 0.39f,
    // 0.38f, 1.00f);
    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered] =
        ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered] =
        ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
    // style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f,
    // 0.95f, 0.73f);
}

} // namespace kzn