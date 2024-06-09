#pragma once

#include "editor/editor_render_system.hpp"
#include "events/event_manager.hpp"
#include <core/window.hpp>
#include <graphics/render_image.hpp>
#include <string_view>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

namespace kzn {

class EditorWindow : public Window {
public:
    // Ctor
    EditorWindow(std::string_view name, int width, int height)
        : Window(name, width, height) {
        // EventManager::listen<RenderSystemInit>(
        //     EventHandler(this, &EditorWindow::on_render_init)
        // );
    }
    // Copy
    EditorWindow(const EditorWindow&) = delete;
    EditorWindow& operator=(const EditorWindow&) = delete;
    // Move
    EditorWindow(EditorWindow&&) = delete;
    EditorWindow& operator=(EditorWindow&&) = delete;
    // Dtor
    ~EditorWindow() = default;

    void set_theme();

    // void on_render_init(const RenderSystemInit&);

    void update(float delta_time);

private:
    // Render target data
    // ImTextureID m_render_image_tex_id;
    // RenderImage m_render_image;
    // VkExtent2D m_viewport_extent;
};

} // namespace kzn