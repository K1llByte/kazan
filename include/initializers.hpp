#ifndef INITIALIZERS_H
#define INITIALIZERS_H

#include "types.hpp"

namespace kzn
{

VkCommandPoolCreateInfo command_pool_create_info(uint32_t queue_family_index, VkCommandPoolCreateFlags flags = 0);

VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool pool, uint32_t count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);


VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(VkShaderStageFlagBits stage, VkShaderModule shader_module);

VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info();

VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info(VkPrimitiveTopology topology);

VkPipelineRasterizationStateCreateInfo rasterization_state_create_info(VkPolygonMode polygon_mode);

VkPipelineMultisampleStateCreateInfo multisampling_state_create_info();

VkPipelineColorBlendAttachmentState color_blend_attachment_state();

VkPipelineLayoutCreateInfo pipeline_layout_create_info();


VkImageCreateInfo image_create_info(VkFormat format, VkImageUsageFlags usage_flags, VkExtent3D extent);

VkImageViewCreateInfo imageview_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspect_flags);

VkPipelineDepthStencilStateCreateInfo depth_stencil_create_info(bool b_depth_test, bool b_depth_write, VkCompareOp compare_op);

}

#endif // INITIALIZERS_H