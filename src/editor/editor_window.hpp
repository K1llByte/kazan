#pragma once

#include "core/log.hpp"
#include "ecs/entity.hpp"
#include "events/event_manager.hpp"
#include "glm/fwd.hpp"
#include "graphics/passes/offscreen_pass.hpp"
#include "graphics/renderer.hpp"
#include "graphics/utils.hpp"
#include "math/types.hpp"
#include "vk/render_pass.hpp"
#include <core/window.hpp>
#include <cstddef>
#include <graphics/render_image.hpp>
#include <memory>
#include <optional>
#include <string_view>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <type_traits>
#include <utility>
#include <vulkan/vulkan_core.h>

namespace kzn {

struct Panel {
    virtual ~Panel() = default;
    virtual void render() = 0;
};

//! Inspector of entities, allows to modify entity data
class InspectorPanel : public Panel {
public:
    InspectorPanel(Vec3& tmp)
        : m_tmp(tmp) {}
    ~InspectorPanel() = default;

    void inspect(Entity entity) { m_entity = entity; }

    void render() override;

private:
    Vec3& m_tmp;
    std::optional<Entity> m_entity = std::nullopt;
};

//! Viewport panel for rendering the scene
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

//! This class is responsible for initializing the Imgui context for vulkan.
//! Its used directly by the EditorPass. This context is initialized here so
//! that other may be added and immediately initialize render related data such
//! as the viewport RenderImage.
class EditorRenderContext {
public:
    // Ctor
    EditorRenderContext(Window& window);
    // Copy
    EditorRenderContext(const EditorRenderContext&) = delete;
    EditorRenderContext& operator=(const EditorRenderContext&) = delete;
    // Move
    EditorRenderContext(EditorRenderContext&&) = delete;
    EditorRenderContext& operator=(EditorRenderContext&&) = delete;
    // Dtor
    ~EditorRenderContext();

    [[nodiscard]]
    vk::RenderPass& render_pass() {
        return m_editor_render_pass;
    }
    [[nodiscard]]
    VkDescriptorPool dset_pool() {
        return m_imgui_pool;
    }

private:
    VkDescriptorPool m_imgui_pool = VK_NULL_HANDLE;
    vk::RenderPass m_editor_render_pass;
};

//! Main editor window abstraction, a placeholder to attach and compose panels.
//! Lifetime of EditorWindow should always outlive the Renderer, as it's a hard
//! dependency on creation and destruction.
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

    [[nodiscard]]
    Window& root_window() {
        return m_window;
    }
    [[nodiscard]]
    EditorRenderContext& render_context() {
        return m_render_context;
    }

    void render();

    template<typename P, typename... Args>
        requires std::is_base_of_v<Panel, P>
    P& add_panel(Args&&... args) {
        m_panels.push_back(std::make_unique<P>(std::forward<Args>(args)...));
        return static_cast<P&>(*m_panels.back());
    }
    // TODO: Query/Remove panels

private:
    Window& m_window;
    EditorRenderContext m_render_context;
    std::vector<std::unique_ptr<Panel>> m_panels;

private:
    void set_theme();
};

} // namespace kzn