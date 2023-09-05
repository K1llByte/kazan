#pragma once

#include "cmd_buffer.hpp"

namespace kzn::vk {

constexpr VkViewport create_viewport(VkExtent2D extent) {
    return VkViewport {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(extent.width),
        .height = static_cast<float>(extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
}

constexpr VkRect2D create_scissor(VkExtent2D extent) {
    return VkRect2D{
        { 0, 0 },
        extent
    };
}

void cmd_set_viewport(CommandBuffer& cmd_buffer, VkViewport viewport);

void cmd_set_scissor(CommandBuffer& cmd_buffer, VkRect2D scissor);

} // namespace kzn
