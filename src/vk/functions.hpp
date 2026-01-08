#pragma once

#include "cmd_buffer.hpp"
#include "core/log.hpp"
#include "core/type.hpp"
#include "vk/dset.hpp"
#include <functional>
#include <ranges>
#include <vulkan/vulkan_core.h>

namespace kzn::vk {

constexpr VkViewport create_viewport(VkExtent2D extent) {
    return VkViewport{
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(extent.width),
        .height = static_cast<float>(extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
}

constexpr VkRect2D create_scissor(VkExtent2D extent) {
    return VkRect2D{{0, 0}, extent};
}

inline void cmd_draw_indexed(
    CommandBuffer& cmd_buffer,
    uint32_t index_count,
    uint32_t instance_count = 1,
    uint32_t first_index = 0,
    int32_t vertex_offset = 0,
    uint32_t first_instance = 0
) {
    vkCmdDrawIndexed(
        cmd_buffer.vk_cmd_buffer(),
        index_count,
        instance_count,
        first_index,
        vertex_offset,
        first_instance
    );
}

inline void cmd_draw(
    CommandBuffer& cmd_buffer,
    uint32_t vertex_count,
    uint32_t instance_count = 1,
    uint32_t first_vertex = 0,
    uint32_t first_instance = 0
) {
    vkCmdDraw(
        cmd_buffer.vk_cmd_buffer(),
        vertex_count,
        instance_count,
        first_vertex,
        first_instance
    );
}

//! Vulkan command to bind a single descriptor set.
//! \param dsets Array of references to the descriptor sets to bind.
//! \param cmd_buffer Command buffer used for this command.
//! \param pipeline_layout Layout of the pipeline where descriptor sets are
//! going to be bound.
template<std::size_t N>
inline void cmd_bind_dsets(
    std::array<Ref<DescriptorSet>, N> dsets,
    CommandBuffer& cmd_buffer,
    VkPipelineLayout pipeline_layout
) {
    std::array<VkDescriptorSet, N> vk_dsets;
    for (std::size_t i = 0; i < N; ++i) {
        vk_dsets[i] = dsets[i].get().vk_dset();
    }

    vkCmdBindDescriptorSets(
        cmd_buffer.vk_cmd_buffer(),
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline_layout,
        0,
        vk_dsets.size(),
        vk_dsets.data(),
        0,
        nullptr
    );
}

//! Vulkan command to bind a single descriptor set.
//! \param dset Array of references to the descriptor sets to bind.
//! \param cmd_buffer Command buffer used for this command.
//! \param pipeline_layout Layout of the pipeline where descriptor sets are
//! going to be bound.
inline void cmd_bind_dset(
    DescriptorSet& dset,
    CommandBuffer& cmd_buffer,
    VkPipelineLayout pipeline_layout
) {
    cmd_bind_dsets(std::array{Ref(dset)}, cmd_buffer, pipeline_layout);
}

//! Vulkan command to upload push contants to a pipeline.
//! \param cmd_buffer Command buffer used for this command.
//! \param pipeline_layout Layout of the pipeline where this push contants will
//! be available.
//! \param data Data to submit
// TODO: Add type constraits that reflect vulkan runtime constraits for data.
template<typename T>
inline void cmd_push_constants(
    CommandBuffer& cmd_buffer,
    VkPipelineLayout layout,
    const T& data
) {
    vkCmdPushConstants(
        cmd_buffer.vk_cmd_buffer(),
        layout,
        VK_SHADER_STAGE_ALL_GRAPHICS,
        0,
        sizeof(T),
        &data
    );
}

//! Vulkan command to set dynamic rasterization line width.
//! \param cmd_buffer Command buffer used for this command.
//! \param line_width Line width.
inline void cmd_set_line_width(CommandBuffer& cmd_buffer, float line_width) {
    vkCmdSetLineWidth(cmd_buffer.vk_cmd_buffer(), line_width);
}

//! Vulkan command to set dynamic viewport.
//! \param cmd_buffer Command buffer used for this command.
//! \param viewport Viewport values.
inline void cmd_set_viewport(CommandBuffer& cmd_buffer, VkViewport viewport) {
    vkCmdSetViewport(cmd_buffer.vk_cmd_buffer(), 0, 1, &viewport);
}

//! Vulkan command to set dynamic scissor.
//! \param cmd_buffer Command buffer used for this command.
//! \param scissor Scissor values.
inline void cmd_set_scissor(CommandBuffer& cmd_buffer, VkRect2D scissor) {
    vkCmdSetScissor(cmd_buffer.vk_cmd_buffer(), 0, 1, &scissor);
}

} // namespace kzn::vk
