#pragma once

#include "vk/cmd_buffers.hpp"

namespace kzn::vk
{
    void cmd_set_viewport(CommandBuffer& cmd_buffer, VkViewport viewport);
    void cmd_set_scissor(CommandBuffer& cmd_buffer, VkRect2D scissor);
    void cmd_draw(
        CommandBuffer& cmd_buffer,
        uint32_t vertex_count,
        uint32_t instance_count,
        uint32_t first_vertex,
        uint32_t first_instance);
} // namespace kzn::vk