#include "initializers.hpp"

namespace kzn
{

VkCommandPoolCreateInfo command_pool_create_info(uint32_t queue_family_index, VkCommandPoolCreateFlags flags /*= 0*/)
{
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;

    info.queueFamilyIndex = queue_family_index;
    info.flags = flags;
    return info;
}

VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool pool, uint32_t count, VkCommandBufferLevel level)
{
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;

    info.commandPool = pool;
    info.commandBufferCount = count;
    info.level = level;
    return info;
}


VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(VkShaderStageFlagBits stage, VkShaderModule shader_module)
{
    VkPipelineShaderStageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.pNext = nullptr;

    // Shader stage
    info.stage = stage;
    // Module containing the code for this shader stage
    info.module = shader_module;
    // The entry point of the shader
    info.pName = "main";
    return info;
}


VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info()
{
    VkPipelineVertexInputStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    info.pNext = nullptr;

    // No vertex bindings or attributes
    info.vertexBindingDescriptionCount = 0;
    info.vertexAttributeDescriptionCount = 0;
    return info;
}


VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info(VkPrimitiveTopology topology)
{
    VkPipelineInputAssemblyStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    info.pNext = nullptr;

    info.topology = topology;
    // We are not going to use primitive restart on the entire tutorial so leave it on false
    info.primitiveRestartEnable = VK_FALSE;
    return info;
}


VkPipelineRasterizationStateCreateInfo rasterization_state_create_info(VkPolygonMode polygon_mode)
{
    VkPipelineRasterizationStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    info.pNext = nullptr;

    info.depthClampEnable = VK_FALSE;
    // Discards all primitives before the rasterization stage if enabled which we don't want
    info.rasterizerDiscardEnable = VK_FALSE;

    info.polygonMode = polygon_mode;
    info.lineWidth = 1.0f;
    // No backface cull
    info.cullMode = VK_CULL_MODE_NONE; // VK_CULL_MODE_BACK_BIT;
    info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    // info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    // No depth bias
    info.depthBiasEnable = VK_FALSE;
    info.depthBiasConstantFactor = 0.0f;
    info.depthBiasClamp = 0.0f;
    info.depthBiasSlopeFactor = 0.0f;

    return info;
}


VkPipelineMultisampleStateCreateInfo multisampling_state_create_info()
{
    VkPipelineMultisampleStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    info.pNext = nullptr;

    info.sampleShadingEnable = VK_FALSE;
    // Multisampling defaulted to no multisampling (1 sample per pixel)
    info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    info.minSampleShading = 1.0f;
    info.pSampleMask = nullptr;
    info.alphaToCoverageEnable = VK_FALSE;
    info.alphaToOneEnable = VK_FALSE;
    return info;
}


VkPipelineColorBlendAttachmentState color_blend_attachment_state()
{
    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
                                          | VK_COLOR_COMPONENT_G_BIT
                                          | VK_COLOR_COMPONENT_B_BIT
                                          | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;
    return color_blend_attachment;
}


VkPipelineLayoutCreateInfo pipeline_layout_create_info()
{
    VkPipelineLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.pNext = nullptr;

    // Empty defaults
    info.flags = 0;
    info.setLayoutCount = 0;
    info.pSetLayouts = nullptr;
    info.pushConstantRangeCount = 0;
    info.pPushConstantRanges = nullptr;
    return info;
}


VkImageCreateInfo image_create_info(VkFormat format, VkImageUsageFlags usage_flags, VkExtent3D extent)
{
    VkImageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;

    info.imageType = VK_IMAGE_TYPE_2D;

    info.format = format;
    info.extent = extent;

    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usage_flags;

    return info;
}


VkImageViewCreateInfo imageview_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspect_flags)
{
    // Build a image-view for the depth image to use for rendering
    VkImageViewCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.pNext = nullptr;

    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.image = image;
    info.format = format;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;
    info.subresourceRange.aspectMask = aspect_flags;

    return info;
}

}