#pragma once

#include "vk/render_pass.hpp"
#include "vk/swapchain.hpp"

#include <vector>

namespace kzn {

std::vector<vk::Framebuffer> create_swapchain_framebuffers(
    vk::RenderPass& render_pass,
    vk::Swapchain& swapchain);

vk::RenderPass simple_pass(vk::Device& device, VkFormat color_format);

} // namespace kzn
