#include "vk/pipeline.hpp"

// #include <fstream>

namespace kzn::vk
{
    PipelineConfigBuilder::PipelineConfigBuilder(
        VkPipelineLayout layout,
        VkRenderPass render_pass)
        : config{}
    {
        config.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        config.input_assembly_info.primitiveRestartEnable = VK_FALSE;

        config.viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        config.viewport_info.viewportCount = 1;
        config.viewport_info.pViewports = nullptr;
        config.viewport_info.scissorCount = 1;
        config.viewport_info.pScissors = nullptr;

        config.rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        config.rasterization_info.depthClampEnable = VK_FALSE;
        config.rasterization_info.rasterizerDiscardEnable = VK_FALSE;
        config.rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
        config.rasterization_info.lineWidth = 1.0f;
        config.rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT; //VK_CULL_MODE_NONE;
        config.rasterization_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // VK_FRONT_FACE_CLOCKWISE;
        config.rasterization_info.depthBiasEnable = VK_FALSE;
        config.rasterization_info.depthBiasConstantFactor = 0.0f;  // Optional
        config.rasterization_info.depthBiasClamp = 0.0f;           // Optional
        config.rasterization_info.depthBiasSlopeFactor = 0.0f;     // Optional

        config.multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        config.multisample_info.sampleShadingEnable = VK_FALSE;
        config.multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        config.multisample_info.minSampleShading = 1.0f;           // Optional
        config.multisample_info.pSampleMask = nullptr;             // Optional
        config.multisample_info.alphaToCoverageEnable = VK_FALSE;  // Optional
        config.multisample_info.alphaToOneEnable = VK_FALSE;       // Optional

        config.color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
                                                          | VK_COLOR_COMPONENT_G_BIT
                                                          | VK_COLOR_COMPONENT_B_BIT
                                                          | VK_COLOR_COMPONENT_A_BIT;
        config.color_blend_attachment.blendEnable = VK_FALSE;
        config.color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        config.color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        config.color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
        config.color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        config.color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        config.color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

        config.color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        config.color_blend_info.logicOpEnable = VK_FALSE;
        config.color_blend_info.logicOp = VK_LOGIC_OP_COPY;  // Optional
        config.color_blend_info.attachmentCount = 1;
        config.color_blend_info.pAttachments = &config.color_blend_attachment;
        config.color_blend_info.blendConstants[0] = 0.0f;  // Optional
        config.color_blend_info.blendConstants[1] = 0.0f;  // Optional
        config.color_blend_info.blendConstants[2] = 0.0f;  // Optional
        config.color_blend_info.blendConstants[3] = 0.0f;  // Optional

        config.depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        config.depth_stencil_info.depthTestEnable = VK_TRUE;
        config.depth_stencil_info.depthWriteEnable = VK_TRUE;
        config.depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
        config.depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
        config.depth_stencil_info.minDepthBounds = 0.0f;  // Optional
        config.depth_stencil_info.maxDepthBounds = 1.0f;  // Optional
        config.depth_stencil_info.stencilTestEnable = VK_FALSE;
        config.depth_stencil_info.front = {};  // Optional
        config.depth_stencil_info.back = {};   // Optional

        config.dynamic_state_enables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        config.dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        config.dynamic_state_info.pDynamicStates = config.dynamic_state_enables.data();
        config.dynamic_state_info.dynamicStateCount =
            static_cast<uint32_t>(config.dynamic_state_enables.size());
        config.dynamic_state_info.flags = 0;

        config.pipeline_layout = layout;
        config.render_pass = render_pass;
    }

    PipelineConfigBuilder& PipelineConfigBuilder::set_layout(VkPipelineLayout layout)
    {
        config.pipeline_layout = layout;
        return *this;
    }


    PipelineConfigBuilder& PipelineConfigBuilder::set_render_pass(VkRenderPass render_pass)
    {
        config.render_pass = render_pass;
        return *this;
    }


    PipelineConfigBuilder& PipelineConfigBuilder::set_topology(VkPrimitiveTopology topology)
    {
        config.input_assembly_info.topology = topology;
        return *this;
    }

    PipelineConfigBuilder& PipelineConfigBuilder::set_polygon_mode(VkPolygonMode polygon_mode)
    {
        config.rasterization_info.polygonMode = polygon_mode;
        return *this;
    }


    PipelineConfig&& PipelineConfigBuilder::build()
    {
        if(config.render_pass == VK_NULL_HANDLE
            || config.pipeline_layout == VK_NULL_HANDLE)
        {
            // TODO: Since these are required parameters this exception shouldn't exist
            throw std::runtime_error("'VkPipelineLayout' and 'VkRenderPass' must be set");
        }
        return std::move(config);
    }
}