#pragma once

#include <core/window.hpp>
#include <string_view>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

namespace kzn {

class EditorWindow : public Window {
public:
    // Ctor
    EditorWindow(std::string_view name, int width, int height)
        : Window(name, width, height) {}
    // Copy
    EditorWindow(const EditorWindow&) = delete;
    EditorWindow& operator=(const EditorWindow&) = delete;
    // Move
    EditorWindow(EditorWindow&&) = delete;
    EditorWindow& operator=(EditorWindow&&) = delete;

    // Dtor
    ~EditorWindow() = default;

    void set_theme() {
        ImGuiStyle* style = &ImGui::GetStyle();

        // SRGB Palette
        // // const auto AQUA         = ImVec4(0.408f, 0.616f, 0.416f, 1.f); //
        // 689d6a
        // const auto DEBUG = ImVec4(1.00f, 0.00f, 0.00f, 1.00f); // ff0000
        const auto DARK_GREY =
            ImVec4(0.004753f, 0.004006f, 0.006017f, 1.00f); // 0f0d12
        const auto DARK_GREY_HOVER =
            ImVec4(0.046964f, 0.043233f, 0.068384f, 1.00f);
        const auto GREY =
            ImVec4(0.010022f, 0.008540f, 0.013411f, 1.00f); // 1a171f
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
        style->Colors[ImGuiCol_BorderShadow] =
            ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
        style->Colors[ImGuiCol_FrameBg] = GREY;
        style->Colors[ImGuiCol_FrameBgHovered] = DARK_GREY_HOVER;
        style->Colors[ImGuiCol_FrameBgActive] = GREY_HOVER;
        style->Colors[ImGuiCol_TitleBg] = GREY;
        style->Colors[ImGuiCol_TitleBgCollapsed] =
            ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
        style->Colors[ImGuiCol_TitleBgActive] =
            ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
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
        style->Colors[ImGuiCol_PlotHistogram] =
            ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
        style->Colors[ImGuiCol_PlotHistogramHovered] =
            ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        style->Colors[ImGuiCol_TextSelectedBg] =
            ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
        // style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f,
        // 0.95f, 0.73f);
    }

    void update(float delta_time) {
        // Draw commands
        ImGui::DockSpaceOverViewport();
        ImGui::ShowDemoWindow();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Viewport");
        ImGui::PopStyleVar();
        ImGui::End();
    }
};

} // namespace kzn