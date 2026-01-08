#pragma once

#include "vk/render_pass.hpp"
#include "vk/swapchain.hpp"

#include <vector>

namespace kzn {

std::vector<vk::Framebuffer> create_swapchain_framebuffers(
    vk::RenderPass& render_pass,
    vk::Swapchain& swapchain
);

//! Create a render pass with one attachment for presentation and clear on load
vk::RenderPass simple_pass(vk::Device& device, VkFormat color_format);
//! Create a render pass with two attachments. One for presentation and clear on
//! load, and another for depth testing.
vk::RenderPass simple_depth_pass(vk::Device& device, VkFormat color_format);
//! Create a render pass with one attachment for shader read and clear on
//! load.
vk::RenderPass simple_offscreen_pass(vk::Device& device, VkFormat color_format);

} // namespace kzn
