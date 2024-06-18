// #pragma once

// #include "ecs/system.hpp"
// #include "graphics/graphics_context.hpp"
// #include "graphics/render_image.hpp"
// #include "graphics/renderer.hpp"
// #include "vk/pipeline.hpp"

// #include "editor_window.hpp"

// namespace kzn {

// //! Notifies when RenderSystem is initialized
// struct RenderSystemInit : Event {};

// class EditorWindow;

// //! ECS System for rendering with ImGui
// class EditorRenderSystem : public System {
// public:
//     // Ctor
//     EditorRenderSystem(EditorWindow& window);
//     // Copy
//     EditorRenderSystem(const EditorRenderSystem&) = delete;
//     EditorRenderSystem& operator=(const EditorRenderSystem&) = delete;
//     // Move
//     EditorRenderSystem(EditorRenderSystem&&) = delete;
//     EditorRenderSystem& operator=(EditorRenderSystem&&) = delete;
//     // Dtor
//     ~EditorRenderSystem() override;

//     void update(float delta_time) override;

//     void on_swapchain_resize(const SwapchainResize&);

// private:
//     EditorWindow& m_window;
//     Renderer m_renderer;
//     RenderImage m_render_target;
//     vk::RenderPass m_main_render_pass;
//     std::vector<vk::Framebuffer> m_framebuffers;
//     VkDescriptorPool m_imgui_pool;
// };

// } // namespace kzn