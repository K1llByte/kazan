#include "functions.hpp"

namespace kzn::vk {

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
