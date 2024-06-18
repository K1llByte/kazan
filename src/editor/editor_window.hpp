#pragma once

#include "events/event_manager.hpp"
#include "graphics/renderer.hpp"
#include "graphics/utils.hpp"
#include "vk/render_pass.hpp"
#include <core/window.hpp>
#include <cstddef>
#include <graphics/render_image.hpp>
#include <memory>
#include <string_view>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <type_traits>
#include <utility>
#include <vulkan/vulkan_core.h>

namespace kzn {

struct Panel {
    virtual void render() = 0;
};

class ViewportPanel : public Panel {
public:
    ViewportPanel();
    ~ViewportPanel();

    void render() override;

    [[nodiscard]]
    RenderImage& render_image() {
        return m_render_image;
    }
    [[nodiscard]]
    VkExtent2D viewport_extent() {
        return m_viewport_extent;
    }

private:
    // Render target data
    ImTextureID m_render_image_tex_id = nullptr;
    RenderImage m_render_image;
    VkExtent2D m_viewport_extent;
};

class EditorWindow {
public:
    // Ctor
    EditorWindow(Window& window);
    // Copy
    EditorWindow(const EditorWindow&) = delete;
    EditorWindow& operator=(const EditorWindow&) = delete;
    // Move
    EditorWindow(EditorWindow&&) = delete;
    EditorWindow& operator=(EditorWindow&&) = delete;
    // Dtor
    ~EditorWindow() = default;

    void render();

    [[nodiscard]]
    Window& root_window() {
        return m_window;
    }

    template<typename P, typename... Args>
        requires std::is_base_of_v<Panel, P>
    P& add_panel(Args&&... args) {
        m_panels.push_back(std::make_unique<P>(std::forward(args)...));
        return static_cast<P&>(*m_panels.back());
    }

private:
    Window& m_window;
    std::vector<std::unique_ptr<Panel>> m_panels;

private:
    void set_theme();
};

} // namespace kzn