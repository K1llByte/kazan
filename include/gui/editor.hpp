#pragma once

#include "vk/cmd_buffers.hpp"
#include "core/passes/screen_depth_pass.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

namespace kzn {
    class Editor
    {
        public:
        Editor(Window& _window, ScreenDepthPass& screen_pass);
        ~Editor();

        inline VkImage viewport_image() const { return m_render_target_image; }

        void draw(vk::CommandBuffer& cmd_buffer);

        private:
        VkDescriptorPool m_imgui_pool;
        // Render Target
        ImTextureID   m_render_target_tex_id;
        VkImage       m_render_target_image;
        VmaAllocation m_render_target_allocation;
        VkImageView   m_render_target_image_view;
        VkSampler     m_render_target_sampler;
        // Viewport
        ImVec2 m_viewport_extent;

        private:
        void set_theme();
        bool handle_viewport_resize();
    };
}