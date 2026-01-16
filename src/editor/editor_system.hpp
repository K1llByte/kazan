#pragma once

#include "core/console.hpp"
#include "core/log.hpp"
#include "core/window.hpp"
#include "ecs/system.hpp"
#include "editor/console_panel.hpp"
#include "editor/panel.hpp"
#include "editor/test_panel.hpp"
#include "input/input.hpp"
#include "vk/cmd_buffer.hpp"

#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>

#include <memory>
#include <type_traits>
#include <utility>

namespace kzn {

class EditorSystem : public System {
public:
    // Ctor
    EditorSystem(Window& window, Input& input, Console& console)
        : m_input_ptr{&input}
        , m_console_ptr{&console} {
        // Initialize ImGui before RenderSystem
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |=
            (ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_IsSRGB);

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForVulkan(window.glfw_ptr(), true);

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
        io.Fonts->AddFontFromFileTTF("assets/fonts/ruda.bold.ttf", 18.0f);

        // set_theme();
        ImGui::StyleColorsDark();

        // Initialize default panels
        emplace_panel<ConsolePanel>(window, *m_console_ptr);
        emplace_panel<TestPanel>();
    }
    // Copy
    EditorSystem(const EditorSystem&) = delete;
    EditorSystem& operator=(const EditorSystem&) = delete;
    // Move
    EditorSystem(EditorSystem&&) = delete;
    EditorSystem& operator=(EditorSystem&&) = delete;
    // Dtor
    ~EditorSystem() = default;

    void update(float delta_time) override {
        // Start preparing Ui state for rendering
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        // Update panels
        for (auto& panel_ptr : m_panels) {
            panel_ptr->update(delta_time);
        }

        // Render dear imgui into screen
        ImGui::Render();
    }

    template<typename T, typename... Args>
        requires std::is_base_of_v<Panel, T>
    T& emplace_panel(Args&&... args) {
        m_panels.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        return static_cast<T&>(*m_panels.back());
    }

    void set_theme() {
        // auto& colors = ImGui::GetStyle().Colors;
        // colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.1f, 0.13f, 1.0f};
        // colors[ImGuiCol_MenuBarBg] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

        // // Border
        // colors[ImGuiCol_Border] = ImVec4{0.44f, 0.37f, 0.61f, 0.29f};
        // colors[ImGuiCol_BorderShadow] = ImVec4{0.0f, 0.0f, 0.0f, 0.24f};

        // // Text
        // colors[ImGuiCol_Text] = ImVec4{1.0f, 1.0f, 1.0f, 1.0f};
        // colors[ImGuiCol_TextDisabled] = ImVec4{0.5f, 0.5f, 0.5f, 1.0f};

        // // Headers
        // colors[ImGuiCol_Header] = ImVec4{0.13f, 0.13f, 0.17, 1.0f};
        // colors[ImGuiCol_HeaderHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
        // colors[ImGuiCol_HeaderActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

        // // Buttons
        // colors[ImGuiCol_Button] = ImVec4{0.13f, 0.13f, 0.17, 1.0f};
        // colors[ImGuiCol_ButtonHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
        // colors[ImGuiCol_ButtonActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
        // colors[ImGuiCol_CheckMark] = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};

        // // Popups
        // colors[ImGuiCol_PopupBg] = ImVec4{0.1f, 0.1f, 0.13f, 0.92f};

        // // Slider
        // colors[ImGuiCol_SliderGrab] = ImVec4{0.44f, 0.37f, 0.61f, 0.54f};
        // colors[ImGuiCol_SliderGrabActive] = ImVec4{0.74f, 0.58f, 0.98f,
        // 0.54f};

        // // Frame BG
        // colors[ImGuiCol_FrameBg] = ImVec4{0.13f, 0.13, 0.17, 1.0f};
        // colors[ImGuiCol_FrameBgHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
        // colors[ImGuiCol_FrameBgActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

        // // Tabs
        // colors[ImGuiCol_Tab] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
        // colors[ImGuiCol_TabHovered] = ImVec4{0.24, 0.24f, 0.32f, 1.0f};
        // colors[ImGuiCol_TabActive] = ImVec4{0.2f, 0.22f, 0.27f, 1.0f};
        // colors[ImGuiCol_TabUnfocused] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
        // colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.16f, 0.16f,
        // 0.21f, 1.0f};

        // // Title
        // colors[ImGuiCol_TitleBg] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
        // colors[ImGuiCol_TitleBgActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
        // colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.16f, 0.16f,
        // 0.21f, 1.0f};

        // // Scrollbar
        // colors[ImGuiCol_ScrollbarBg] = ImVec4{0.1f, 0.1f, 0.13f, 1.0f};
        // colors[ImGuiCol_ScrollbarGrab] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
        // colors[ImGuiCol_ScrollbarGrabHovered] =
        //     ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
        // colors[ImGuiCol_ScrollbarGrabActive] =
        //     ImVec4{0.24f, 0.24f, 0.32f, 1.0f};

        // // Seperator
        // colors[ImGuiCol_Separator] = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};
        // colors[ImGuiCol_SeparatorHovered] = ImVec4{0.74f, 0.58f,
        // 0.98f, 1.0f}; colors[ImGuiCol_SeparatorActive] = ImVec4{0.84f,
        // 0.58f, 1.0f, 1.0f};

        // // Resize Grip
        // colors[ImGuiCol_ResizeGrip] = ImVec4{0.44f, 0.37f, 0.61f, 0.29f};
        // colors[ImGuiCol_ResizeGripHovered] = ImVec4{0.74f, 0.58f, 0.98f,
        // 0.29f}; colors[ImGuiCol_ResizeGripActive] = ImVec4{0.84f,
        // 0.58f, 1.0f, 0.29f};

        // // Docking
        // colors[ImGuiCol_DockingPreview] = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};

        // auto& style = ImGui::GetStyle();
        // style.TabRounding = 4;
        // style.ScrollbarRounding = 9;
        // style.WindowRounding = 7;
        // style.GrabRounding = 3;
        // style.FrameRounding = 3;
        // style.PopupRounding = 4;
        // style.ChildRounding = 4;

        ImGuiStyle* style = &ImGui::GetStyle();
        ImVec4* colors = style->Colors;

        auto debug_color = ImVec4(1.00, 0.00, 0.00, 1.00);

        colors[ImGuiCol_Text] = ImVec4(1.00, 1.00, 1.00, 1.00);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.21, 0.21, 0.21, 1.00);
        colors[ImGuiCol_WindowBg] = ImVec4(0.00, 0.00, 0.00, 0.94);
        colors[ImGuiCol_ChildBg] = ImVec4(0.00, 0.00, 0.00, 0.00);
        colors[ImGuiCol_PopupBg] = ImVec4(0.01, 0.01, 0.01, 0.94);
        colors[ImGuiCol_Border] = ImVec4(0.15, 0.15, 0.21, 0.50);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00, 0.00, 0.00, 0.00);
        colors[ImGuiCol_FrameBg] = ImVec4(0.02, 0.07, 0.20, 0.54);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.05, 0.31, 0.96, 0.40);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.05, 0.31, 0.96, 0.67);
        colors[ImGuiCol_TitleBg] = ImVec4(0.00, 0.00, 0.00, 1.00);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.02, 0.07, 0.20, 1.00);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00, 0.00, 0.00, 0.51);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.02, 0.02, 0.02, 1.00);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00, 0.00, 0.00, 0.53);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.08, 0.08, 0.08, 1.00);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.14, 0.14, 0.14, 1.00);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.22, 0.22, 0.22, 1.00);
        colors[ImGuiCol_CheckMark] = ImVec4(0.05, 0.31, 0.96, 1.00);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.05, 0.23, 0.75, 1.00);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.05, 0.31, 0.96, 1.00);
        colors[ImGuiCol_Button] = ImVec4(0.05, 0.31, 0.96, 0.40);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.05, 0.31, 0.96, 1.00);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.00, 0.24, 0.96, 1.00);
        colors[ImGuiCol_Header] = ImVec4(0.05, 0.31, 0.96, 0.31);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.05, 0.31, 0.96, 0.80);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.05, 0.31, 0.96, 1.00);
        colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.01, 0.13, 0.52, 0.78);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.01, 0.13, 0.52, 1.00);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.05, 0.31, 0.96, 0.20);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.05, 0.31, 0.96, 0.67);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.05, 0.31, 0.96, 0.95);
        colors[ImGuiCol_Tab] = ImLerp(
            colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f
        );
        colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
        colors[ImGuiCol_TabActive] = ImLerp(
            colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f
        );
        colors[ImGuiCol_TabUnfocused] =
            ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
        colors[ImGuiCol_TabUnfocusedActive] =
            ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
        colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_HeaderActive];
        colors[ImGuiCol_DockingPreview].z *= 0.7f;
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.03, 0.03, 0.03, 1.00);
        colors[ImGuiCol_PlotLines] = ImVec4(0.33, 0.33, 0.33, 1.00);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00, 0.15, 0.10, 1.00);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.79, 0.45, 0.00, 1.00);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00, 0.32, 0.00, 1.00);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.03, 0.03, 0.03, 1.00);
        colors[ImGuiCol_TableBorderStrong] =
            ImVec4(0.08, 0.08, 0.10, 1.00); // Prefer using Alpha=1.0 here
        colors[ImGuiCol_TableBorderLight] =
            ImVec4(0.04, 0.04, 0.05, 1.00); // Prefer using Alpha=1.0 here
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00, 0.00, 0.00, 0.00);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00, 1.00, 1.00, 0.06);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.05, 0.31, 0.96, 0.35);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00, 1.00, 0.00, 0.90);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.05, 0.31, 0.96, 1.00);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00, 1.00, 1.00, 0.70);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.60, 0.60, 0.60, 0.20);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.60, 0.60, 0.60, 0.35);
    }

private:
    Input* m_input_ptr;
    Console* m_console_ptr;
    std::vector<std::unique_ptr<Panel>> m_panels;
};

} // namespace kzn