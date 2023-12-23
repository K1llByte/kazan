#pragma once

#include "ecs/system.hpp"
#include "graphics/graphics_context.hpp"
#include "graphics/renderer.hpp"
#include "vk/pipeline.hpp"

#include "editor_window.hpp"

namespace kzn {

//! ECS System for rendering with ImGui
class EditorRenderSystem : public System {
public:
    // Ctor
    EditorRenderSystem(EditorWindow& window);
    // Copy
    EditorRenderSystem(const EditorRenderSystem&) = delete;
    EditorRenderSystem& operator=(const EditorRenderSystem&) = delete;
    // Move
    EditorRenderSystem(EditorRenderSystem&&) = delete;
    EditorRenderSystem& operator=(EditorRenderSystem&&) = delete;
    // Dtor
    ~EditorRenderSystem() override;

    void update(float delta_time) override;

    void on_swapchain_resize(const SwapchainResize&);

private:
    ScopedGraphicsContext m_scoped_gfx_context;
    EditorWindow& m_window;
    Renderer m_renderer;
    vk::RenderPass m_main_render_pass;
    std::vector<vk::Framebuffer> m_framebuffers;
};

} // namespace kzn