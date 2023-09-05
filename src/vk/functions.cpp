#include "functions.hpp"

namespace kzn::vk {

void cmd_draw_indexed(
    CommandBuffer& cmd_buffer,
    uint32_t index_count,
    uint32_t instance_count,
    uint32_t first_index,
    int32_t vertex_offset,
    uint32_t first_instance)
{
    vkCmdDrawIndexed(
        cmd_buffer.vk_cmd_buffer(),
        index_count,
        instance_count,
        first_index,
        vertex_offset,
        first_instance);
}

void cmd_draw(
    CommandBuffer& cmd_buffer,
    uint32_t vertex_count,
    uint32_t instance_count,
    uint32_t first_vertex,
    uint32_t first_instance)
{
    vkCmdDraw(
        cmd_buffer.vk_cmd_buffer(),
        vertex_count,
        instance_count,
        first_vertex,
        first_instance);
}

void cmd_set_viewport(CommandBuffer& cmd_buffer, VkViewport viewport) {
    vkCmdSetViewport(
        cmd_buffer.vk_cmd_buffer(),
        0,
        1,
        &viewport
    );
}

void cmd_set_scissor(CommandBuffer& cmd_buffer, VkRect2D scissor) {
    vkCmdSetScissor(
        cmd_buffer.vk_cmd_buffer(),
        0,
        1,
        &scissor
    );
}

} // namespace kzn::vk
