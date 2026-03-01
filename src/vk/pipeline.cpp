#include "pipeline.hpp"

#include "core/log.hpp"
#include "vk/error.hpp"
#include "vk/shader_code.hpp"

#include <array>
#include <fstream>
#include <vulkan/vulkan_core.h>

namespace kzn::vk {

PipelineConfig::PipelineConfig(const vk::RenderPass& render_pass)
    : m_render_pass(render_pass) {
    m_input_assembly_info = {};
    m_input_assembly_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    m_input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    m_input_assembly_info.primitiveRestartEnable = VK_FALSE;

    // VkViewport viewport{};
    // viewport.x = 0.0f;
    // viewport.y = 0.0f;
    // viewport.width = (float) viewport_extent.width;
    // viewport.height = (float) viewport_extent.height;
    // viewport.minDepth = 0.0f;
    // viewport.maxDepth = 1.0f;
    // m_viewports.push_back(viewport);
    // VkRect2D scissor{};
    // scissor.offset = {0, 0};
    // scissor.extent = viewport_extent;
    // m_scissors.push_back(scissor);

    //
    m_viewport_info = {};
    m_viewport_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    // m_viewport_info.viewportCount = m_viewports.size();
    // m_viewport_info.pViewports = m_viewports.data();
    m_viewport_info.viewportCount = 1;
    m_viewport_info.pViewports = nullptr;
    // m_viewport_info.scissorCount = m_scissors.size();
    // m_viewport_info.pScissors = m_scissors.data();
    m_viewport_info.scissorCount = 1;
    m_viewport_info.pScissors = nullptr;

    m_rasterization_info = {};
    m_rasterization_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    m_rasterization_info.depthClampEnable = VK_FALSE;
    m_rasterization_info.rasterizerDiscardEnable = VK_FALSE;
    m_rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
    m_rasterization_info.lineWidth = 1.0f;
    m_rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT; // VK_CULL_MODE_NONE;
    m_rasterization_info.frontFace =
        VK_FRONT_FACE_CLOCKWISE; // VK_FRONT_FACE_COUNTER_CLOCKWISE;
    m_rasterization_info.depthBiasEnable = VK_FALSE;
    m_rasterization_info.depthBiasConstantFactor = 0.0f; // Optional
    m_rasterization_info.depthBiasClamp = 0.0f;          // Optional
    m_rasterization_info.depthBiasSlopeFactor = 0.0f;    // Optional

    m_multisample_info = {};
    m_multisample_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    m_multisample_info.sampleShadingEnable = VK_FALSE;
    m_multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    m_multisample_info.minSampleShading = 1.0f;          // Optional
    m_multisample_info.pSampleMask = nullptr;            // Optional
    m_multisample_info.alphaToCoverageEnable = VK_FALSE; // Optional
    m_multisample_info.alphaToOneEnable = VK_FALSE;      // Optional

    m_color_blend_attachment = {};
    m_color_blend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    m_color_blend_attachment.blendEnable = VK_TRUE;
    m_color_blend_attachment.srcColorBlendFactor =
        VK_BLEND_FACTOR_SRC_ALPHA; // Optional
    m_color_blend_attachment.dstColorBlendFactor =
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;                 // Optional
    m_color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    m_color_blend_attachment.srcAlphaBlendFactor =
        VK_BLEND_FACTOR_ONE; // Optional
    m_color_blend_attachment.dstAlphaBlendFactor =
        VK_BLEND_FACTOR_ZERO;                                // Optional
    m_color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    m_color_blend_info = {};
    m_color_blend_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    m_color_blend_info.logicOpEnable = VK_FALSE;
    m_color_blend_info.logicOp = VK_LOGIC_OP_COPY; // Optional
    m_color_blend_info.attachmentCount = 1;
    m_color_blend_info.pAttachments = &m_color_blend_attachment;
    m_color_blend_info.blendConstants[0] = 0.0f; // Optional
    m_color_blend_info.blendConstants[1] = 0.0f; // Optional
    m_color_blend_info.blendConstants[2] = 0.0f; // Optional
    m_color_blend_info.blendConstants[3] = 0.0f; // Optional

    m_depth_stencil_info = {};
    m_depth_stencil_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    m_depth_stencil_info.depthTestEnable = VK_TRUE;
    m_depth_stencil_info.depthWriteEnable = VK_TRUE;
    m_depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
    m_depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
    m_depth_stencil_info.minDepthBounds = 0.0f; // Optional
    m_depth_stencil_info.maxDepthBounds = 1.0f; // Optional
    m_depth_stencil_info.stencilTestEnable = VK_FALSE;
    m_depth_stencil_info.front = {}; // Optional
    m_depth_stencil_info.back = {};  // Optional

    // By default VK_DYNAMIC_STATE_VIEWPORT is enabled
    m_dynamic_state_info = {};
    // m_dynamic_state_enables = { VK_DYNAMIC_STATE_VIEWPORT };
    m_dynamic_state_enables = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };
    m_dynamic_state_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    m_dynamic_state_info.pDynamicStates = m_dynamic_state_enables.data();
    m_dynamic_state_info.dynamicStateCount =
        static_cast<uint32_t>(m_dynamic_state_enables.size());
    m_dynamic_state_info.flags = 0;
}

VkShaderModule create_shader_module(
    Device& device,
    const ShaderCode& shader_code
) {
    // Create shader module from the code
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = shader_code.bytecode.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(shader_code.bytecode.data());

    VkShaderModule shader_module;
    auto result =
        vkCreateShaderModule(device, &create_info, nullptr, &shader_module);
    VK_CHECK_MSG(result, "Failed to load shader module!");
    return shader_module;
}

Pipeline::Pipeline(
    Device& device,
    const PipelineStages& stages,
    const PipelineConfig& config
)
    : m_device{device} {
    // Create pipeline layout
    VkPipelineLayoutCreateInfo pipeline_layout_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = static_cast<uint32_t>(
            config.m_pipeline_layout.descriptor_sets.size()
        ),
        .pSetLayouts = config.m_pipeline_layout.descriptor_sets.data(),
        .pushConstantRangeCount =
            static_cast<uint32_t>(config.m_pipeline_layout.push_constants.size()
            ),
        .pPushConstantRanges = config.m_pipeline_layout.push_constants.data(),
    };

    auto result = vkCreatePipelineLayout(
        m_device, &pipeline_layout_info, nullptr, &m_pipeline_layout
    );
    VK_CHECK_MSG(result, "Failed to create pipeline layout!");

    // Create shader stages
    std::array<VkShaderModule, 5> shader_modules{VK_NULL_HANDLE};
    std::array<VkPipelineShaderStageCreateInfo, 5> shader_stages;
    size_t next_stage_idx = 0;
    
    // Vertex Shader
    KZN_ASSERT_MSG(stages.vertex != nullptr, "Vertex stage is not optional");
    auto vertex_shader_mod = create_shader_module(device, *stages.vertex);
    shader_stages[next_stage_idx].sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[next_stage_idx].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader_stages[next_stage_idx].module = vertex_shader_mod;
    shader_stages[next_stage_idx].pName = "main";
    shader_stages[next_stage_idx].flags = 0;
    shader_stages[next_stage_idx].pNext = nullptr;
    shader_stages[next_stage_idx].pSpecializationInfo = nullptr;
    shader_modules[next_stage_idx] = vertex_shader_mod;
    next_stage_idx += 1;

    // Tesselation Control Shader
    if(stages.tess_control != nullptr) {
        auto tess_control_shader_mod = create_shader_module(device, *stages.tess_control);
        shader_stages[next_stage_idx].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[next_stage_idx].stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        shader_stages[next_stage_idx].module = tess_control_shader_mod;
        shader_stages[next_stage_idx].pName = "main";
        shader_stages[next_stage_idx].flags = 0;
        shader_stages[next_stage_idx].pNext = nullptr;
        shader_stages[next_stage_idx].pSpecializationInfo = nullptr;
        shader_modules[next_stage_idx] = tess_control_shader_mod;
        next_stage_idx += 1;
    }
    // Tesselation Evaluation Shader
    if(stages.tess_evaluation != nullptr) {
        auto tess_evaluation_shader_mod = create_shader_module(device, *stages.tess_evaluation);
        shader_stages[next_stage_idx].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[next_stage_idx].stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        shader_stages[next_stage_idx].module = tess_evaluation_shader_mod;
        shader_stages[next_stage_idx].pName = "main";
        shader_stages[next_stage_idx].flags = 0;
        shader_stages[next_stage_idx].pNext = nullptr;
        shader_stages[next_stage_idx].pSpecializationInfo = nullptr;
        shader_modules[next_stage_idx] = tess_evaluation_shader_mod;
        next_stage_idx += 1;
    }
    // Geometry Shader
    if(stages.geometry != nullptr) {
        auto geometry_shader_mod = create_shader_module(device, *stages.geometry);
        shader_stages[next_stage_idx].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[next_stage_idx].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        shader_stages[next_stage_idx].module = geometry_shader_mod;
        shader_stages[next_stage_idx].pName = "main";
        shader_stages[next_stage_idx].flags = 0;
        shader_stages[next_stage_idx].pNext = nullptr;
        shader_stages[next_stage_idx].pSpecializationInfo = nullptr;
        shader_modules[next_stage_idx] = geometry_shader_mod;
        next_stage_idx += 1;
    }
    // Fragment Shader
    if(stages.fragment != nullptr) {
        auto fragment_shader_mod = create_shader_module(device, *stages.fragment);
        shader_stages[next_stage_idx].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[next_stage_idx].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shader_stages[next_stage_idx].module = fragment_shader_mod;
        shader_stages[next_stage_idx].pName = "main";
        shader_stages[next_stage_idx].flags = 0;
        shader_stages[next_stage_idx].pNext = nullptr;
        shader_stages[next_stage_idx].pSpecializationInfo = nullptr;
        shader_modules[next_stage_idx] = fragment_shader_mod;
        next_stage_idx += 1;
    }

    // Setup vertex input description
    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(config.m_vertex_attributes.size());
    vertex_input_info.vertexBindingDescriptionCount =
        static_cast<uint32_t>(config.m_vertex_bindings.size());
    vertex_input_info.pVertexAttributeDescriptions =
        config.m_vertex_attributes.data();
    vertex_input_info.pVertexBindingDescriptions =
        config.m_vertex_bindings.data();

    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = next_stage_idx;
    pipeline_info.pStages = shader_stages.data();
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &config.m_input_assembly_info;
    pipeline_info.pViewportState = &config.m_viewport_info;
    pipeline_info.pRasterizationState = &config.m_rasterization_info;
    pipeline_info.pMultisampleState = &config.m_multisample_info;
    pipeline_info.pColorBlendState = &config.m_color_blend_info;
    pipeline_info.pDepthStencilState = &config.m_depth_stencil_info;
    pipeline_info.pDynamicState = &config.m_dynamic_state_info;

    pipeline_info.layout = m_pipeline_layout;
    pipeline_info.renderPass = config.m_render_pass;
    pipeline_info.subpass = config.m_subpass;

    pipeline_info.basePipelineIndex = -1;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

    result = vkCreateGraphicsPipelines(
        m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_vk_pipeline
    );

    for (auto shader_module : shader_modules) {
        if(shader_module != VK_NULL_HANDLE) {
            vkDestroyShaderModule(m_device, shader_module, nullptr);
        }
    }

    VK_CHECK_MSG(result, "Failed to create graphics pipeline!");
    Log::trace("Pipeline created");
}

Pipeline::Pipeline(
    Device& device,
    const VkGraphicsPipelineCreateInfo& create_info,
    std::vector<DescriptorSetLayout> sparse_dset_layouts
)
    : m_device{device}
    , m_pipeline_layout{create_info.layout}
    , m_sparse_dset_layouts{std::move(sparse_dset_layouts)}
{
    auto result = vkCreateGraphicsPipelines(
        m_device, VK_NULL_HANDLE, 1, &create_info, nullptr, &m_vk_pipeline
    );
    VK_CHECK_MSG(result, "Failed to create graphics pipeline!");
    Log::trace("Pipeline created");
}

Pipeline::~Pipeline() {
    vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
    vkDestroyPipeline(m_device, m_vk_pipeline, nullptr);
    Log::trace("Pipeline destroyed");
}

} // namespace kzn::vk