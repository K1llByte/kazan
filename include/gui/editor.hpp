#pragma once

#include "vk/cmd_buffers.hpp"
#include "core/passes/screen_depth_pass.hpp"
#include "core/render_image.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

namespace kzn {
    class Editor
    {
        public:
        Editor(Window& _window, ScreenDepthPass& screen_pass);
        ~Editor();

        inline RenderImage* viewport_render_image() { return &m_render_image; }

        void draw(vk::CommandBuffer& cmd_buffer);

        private:
        VkDescriptorPool m_imgui_pool;
        // Render Target
        VkExtent2D    m_viewport_extent;
        ImTextureID   m_render_image_tex_id;
        RenderImage   m_render_image;
        // Viewport

        private:
        void set_theme();
        bool viewport_resized();
    };
}