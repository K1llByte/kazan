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
    
    template<typename T>
    void cmd_push_constants(
        CommandBuffer& cmd_buffer,
        VkPipelineLayout layout,
        const T& data);
} // namespace kzn::vk

////////////////////////////////////////////////
//             Implementation                 //
////////////////////////////////////////////////

namespace kzn
{
    // template<typename T>
    // void cmd_push_constants(
    //     CommandBuffer& cmd_buffer,
    //     VkPipelineLayout layout,
    //     const T& data)
    // {
    //     vkCmdPushConstants(
    //         cmd_buffer.vk_command_buffer(),
    //         layout,
    //         VK_SHADER_STAGE_ALL_GRAPHICS,
    //         0,
    //         sizeof(T),
    //         &data
    //     );
    // }
} // namespace kzn