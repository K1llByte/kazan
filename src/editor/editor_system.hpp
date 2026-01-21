#pragma once

#include "core/console.hpp"
#include "core/window.hpp"
#include "ecs/context.hpp"
#include "ecs/system.hpp"
#include "editor/console_panel.hpp"
#include "editor/demo_panel.hpp"
#include "editor/panel.hpp"
#include "events/event_manager.hpp"
#include "events/events.hpp"
#include "graphics/render_system.hpp"
#include "input/input.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <memory>
#include <type_traits>
#include <utility>

namespace kzn {

class EditorSystem : public System {
public:
    using Before = TypeList<RenderSystem>;

public:
    // Ctor
    EditorSystem(Window& window, Input& input, Console& console) {
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

        set_theme();
        // ImGui::StyleColorsDark();

        // Initialize default panels
        emplace_panel<ConsolePanel>(window, context<Console>());
        emplace_panel<DemoPanel>();

        // This event will inject ImGuiStage into the RenderSystem
        EventManager::send(EditorInitEvent{});
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
    }

    template<typename T, typename... Args>
        requires std::is_base_of_v<Panel, T>
    T& emplace_panel(Args&&... args) {
        m_panels.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        return static_cast<T&>(*m_panels.back());
    }

    void set_theme() {}

private:
    std::vector<std::unique_ptr<Panel>> m_panels;
};

} // namespace kzn