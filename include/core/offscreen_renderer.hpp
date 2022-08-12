#ifndef KZN_OFFSCREEN_RENDERER_HPP
#define KZN_OFFSCREEN_RENDERER_HPP

// Reuse PerFrameData
#include "core/renderer.hpp"

namespace kzn {
    class ImGuiWindow;

    class OffscreenRenderer: public Renderer {
        public:
        OffscreenRenderer(Window* window, ImGuiWindow* imgui_window);
        ~OffscreenRenderer();
        
        void render_frame(std::function<void(vk::CommandBuffer&)>) override;
        virtual const std::vector<VkImage>& get_render_images() override {
            return render_images;
            // return Context::swapchain().images();
        }
        virtual VkExtent2D get_render_extent() override {
            return render_extent;
            // return Context::swapchain().get_extent();
        }

        private:
        ImGuiWindow*               imgui_window;
        std::vector<VkImage>       render_images;
        std::vector<VmaAllocation> render_allocation;
        VkExtent2D                 render_extent;
    };
} // namespace kzn

#endif // #define KZN_OFFSCREEN_RENDERER_HPP