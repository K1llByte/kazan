#include "pipeline.hpp"

#include "model.hpp"

#include <fstream>

namespace kzn
{

PipelineConfigBuilder::PipelineConfigBuilder(
    VkPipelineLayout layout,
    VkRenderPass render_pass)
    : _config{}
{
    _config.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    _config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    _config.input_assembly_info.primitiveRestartEnable = VK_FALSE;

    _config.viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    _config.viewport_info.viewportCount = 1;
    _config.viewport_info.pViewports = nullptr;
    _config.viewport_info.scissorCount = 1;
    _config.viewport_info.pScissors = nullptr;

    _config.rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    _config.rasterization_info.depthClampEnable = VK_FALSE;
    _config.rasterization_info.rasterizerDiscardEnable = VK_FALSE;
    _config.rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
    _config.rasterization_info.lineWidth = 1.0f;
    _config.rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT; //VK_CULL_MODE_NONE;
    _config.rasterization_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // VK_FRONT_FACE_CLOCKWISE;
    _config.rasterization_info.depthBiasEnable = VK_FALSE;
    _config.rasterization_info.depthBiasConstantFactor = 0.0f;  // Optional
    _config.rasterization_info.depthBiasClamp = 0.0f;           // Optional
    _config.rasterization_info.depthBiasSlopeFactor = 0.0f;     // Optional

    _config.multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    _config.multisample_info.sampleShadingEnable = VK_FALSE;
    _config.multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    _config.multisample_info.minSampleShading = 1.0f;           // Optional
    _config.multisample_info.pSampleMask = nullptr;             // Optional
    _config.multisample_info.alphaToCoverageEnable = VK_FALSE;  // Optional
    _config.multisample_info.alphaToOneEnable = VK_FALSE;       // Optional

    _config.color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
                                                      | VK_COLOR_COMPONENT_G_BIT
                                                      | VK_COLOR_COMPONENT_B_BIT
                                                      | VK_COLOR_COMPONENT_A_BIT;
    _config.color_blend_attachment.blendEnable = VK_FALSE;
    _config.color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    _config.color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    _config.color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
    _config.color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    _config.color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    _config.color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

    _config.color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    _config.color_blend_info.logicOpEnable = VK_FALSE;
    _config.color_blend_info.logicOp = VK_LOGIC_OP_COPY;  // Optional
    _config.color_blend_info.attachmentCount = 1;
    _config.color_blend_info.pAttachments = &_config.color_blend_attachment;
    _config.color_blend_info.blendConstants[0] = 0.0f;  // Optional
    _config.color_blend_info.blendConstants[1] = 0.0f;  // Optional
    _config.color_blend_info.blendConstants[2] = 0.0f;  // Optional
    _config.color_blend_info.blendConstants[3] = 0.0f;  // Optional

    _config.depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    _config.depth_stencil_info.depthTestEnable = VK_TRUE;
    _config.depth_stencil_info.depthWriteEnable = VK_TRUE;
    _config.depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
    _config.depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
    _config.depth_stencil_info.minDepthBounds = 0.0f;  // Optional
    _config.depth_stencil_info.maxDepthBounds = 1.0f;  // Optional
    _config.depth_stencil_info.stencilTestEnable = VK_FALSE;
    _config.depth_stencil_info.front = {};  // Optional
    _config.depth_stencil_info.back = {};   // Optional

    _config.dynamic_state_enables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    _config.dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    _config.dynamic_state_info.pDynamicStates = _config.dynamic_state_enables.data();
    _config.dynamic_state_info.dynamicStateCount =
        static_cast<uint32_t>(_config.dynamic_state_enables.size());
    _config.dynamic_state_info.flags = 0;

    _config.pipeline_layout = layout;
    _config.render_pass = render_pass;
}


PipelineConfigBuilder& PipelineConfigBuilder::set_layout(VkPipelineLayout layout)
{
    _config.pipeline_layout = layout;
    return *this;
}


PipelineConfigBuilder& PipelineConfigBuilder::set_render_pass(VkRenderPass render_pass)
{
    _config.render_pass = render_pass;
    return *this;
}


PipelineConfigBuilder& PipelineConfigBuilder::set_topology(VkPrimitiveTopology topology)
{
    _config.input_assembly_info.topology = topology;
    return *this;
}


PipelineConfig&& PipelineConfigBuilder::build()
{
    if(_config.render_pass == VK_NULL_HANDLE
        || _config.pipeline_layout == VK_NULL_HANDLE)
    {
        throw std::runtime_error("'VkPipelineLayout' and 'VkRenderPass' must be set");
    }
    return std::move(_config);
}


Pipeline::Pipeline(
        Device& device,
        const std::string& vert_shader_path,
        const std::string& frag_shader_path,
        const PipelineConfig& _config)
    : _device{device}
{
    create_graphics_pipeline(vert_shader_path, frag_shader_path, _config);
}


Pipeline::~Pipeline()
{
    vkDestroyShaderModule(_device.device(), _vert_shader_module, nullptr);
    vkDestroyShaderModule(_device.device(), _frag_shader_module, nullptr);
    vkDestroyPipeline(_device.device(), _graphics_pipeline, nullptr);
}


void Pipeline::bind(VkCommandBuffer command_buffer)
{
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphics_pipeline);
}


void Pipeline::default_pipeline_config_info(PipelineConfig& config_info)
{
    config_info.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    config_info.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    config_info.input_assembly_info.primitiveRestartEnable = VK_FALSE;

    config_info.viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    config_info.viewport_info.viewportCount = 1;
    config_info.viewport_info.pViewports = nullptr;
    config_info.viewport_info.scissorCount = 1;
    config_info.viewport_info.pScissors = nullptr;

    config_info.rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    config_info.rasterization_info.depthClampEnable = VK_FALSE;
    config_info.rasterization_info.rasterizerDiscardEnable = VK_FALSE;
    config_info.rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
    config_info.rasterization_info.lineWidth = 1.0f;
    config_info.rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT; //VK_CULL_MODE_NONE;
    config_info.rasterization_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // VK_FRONT_FACE_CLOCKWISE;
    config_info.rasterization_info.depthBiasEnable = VK_FALSE;
    config_info.rasterization_info.depthBiasConstantFactor = 0.0f;  // Optional
    config_info.rasterization_info.depthBiasClamp = 0.0f;           // Optional
    config_info.rasterization_info.depthBiasSlopeFactor = 0.0f;     // Optional

    config_info.multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    config_info.multisample_info.sampleShadingEnable = VK_FALSE;
    config_info.multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    config_info.multisample_info.minSampleShading = 1.0f;           // Optional
    config_info.multisample_info.pSampleMask = nullptr;             // Optional
    config_info.multisample_info.alphaToCoverageEnable = VK_FALSE;  // Optional
    config_info.multisample_info.alphaToOneEnable = VK_FALSE;       // Optional

    config_info.color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
                                                      | VK_COLOR_COMPONENT_G_BIT
                                                      | VK_COLOR_COMPONENT_B_BIT
                                                      | VK_COLOR_COMPONENT_A_BIT;
    config_info.color_blend_attachment.blendEnable = VK_FALSE;
    config_info.color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    config_info.color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    config_info.color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
    config_info.color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    config_info.color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    config_info.color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

    config_info.color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    config_info.color_blend_info.logicOpEnable = VK_FALSE;
    config_info.color_blend_info.logicOp = VK_LOGIC_OP_COPY;  // Optional
    config_info.color_blend_info.attachmentCount = 1;
    config_info.color_blend_info.pAttachments = &config_info.color_blend_attachment;
    config_info.color_blend_info.blendConstants[0] = 0.0f;  // Optional
    config_info.color_blend_info.blendConstants[1] = 0.0f;  // Optional
    config_info.color_blend_info.blendConstants[2] = 0.0f;  // Optional
    config_info.color_blend_info.blendConstants[3] = 0.0f;  // Optional

    config_info.depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    config_info.depth_stencil_info.depthTestEnable = VK_TRUE;
    config_info.depth_stencil_info.depthWriteEnable = VK_TRUE;
    config_info.depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
    config_info.depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
    config_info.depth_stencil_info.minDepthBounds = 0.0f;  // Optional
    config_info.depth_stencil_info.maxDepthBounds = 1.0f;  // Optional
    config_info.depth_stencil_info.stencilTestEnable = VK_FALSE;
    config_info.depth_stencil_info.front = {};  // Optional
    config_info.depth_stencil_info.back = {};   // Optional

    config_info.dynamic_state_enables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    config_info.dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    config_info.dynamic_state_info.pDynamicStates = config_info.dynamic_state_enables.data();
    config_info.dynamic_state_info.dynamicStateCount =
        static_cast<uint32_t>(config_info.dynamic_state_enables.size());
    config_info.dynamic_state_info.flags = 0;
}


VkShaderModule Pipeline::create_shader_module(const std::string& file_path)
{
    // 1. Read code from file and store it in a vector
    std::ifstream file{file_path, std::ios::ate | std::ios::binary};

    if(!file.is_open())
    {
        throw std::runtime_error("failed to open file: " + file_path);
    }

    size_t file_size = static_cast<size_t>(file.tellg());
    std::vector<char> code(file_size);

    file.seekg(0);
    file.read(code.data(), file_size);

    file.close();

    // 2. Create shader module from the code
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shader_module;
    if(vkCreateShaderModule(_device.device(), &create_info, nullptr, &shader_module) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module");
    }

    return shader_module;
}


void Pipeline::create_graphics_pipeline(
    const std::string& vert_path,
    const std::string& frag_path,
    const PipelineConfig& _config)
{
    
    if(_config.pipeline_layout == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Cannot create graphics pipeline: no 'pipeline_layout' provided in PipelineConfig");
    }

    if(_config.render_pass == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Cannot create graphics pipeline: no 'render_pass' provided in PipelineConfig");
    }

    _vert_shader_module = create_shader_module(vert_path);
    _frag_shader_module = create_shader_module(frag_path);

    VkPipelineShaderStageCreateInfo shader_stages[2];
    shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader_stages[0].module = _vert_shader_module;
    shader_stages[0].pName = "main";
    shader_stages[0].flags = 0;
    shader_stages[0].pNext = nullptr;
    shader_stages[0].pSpecializationInfo = nullptr;
    shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_stages[1].module = _frag_shader_module;
    shader_stages[1].pName = "main";
    shader_stages[1].flags = 0;
    shader_stages[1].pNext = nullptr;
    shader_stages[1].pSpecializationInfo = nullptr;

    auto descriptions = Model::Vertex::get_vertex_description();
    auto& binding_descriptions = descriptions.bindings;
    auto& attribute_descriptions = descriptions.attributes;
    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(attribute_descriptions.size());
    vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(binding_descriptions.size());
    vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();
    vertex_input_info.pVertexBindingDescriptions = binding_descriptions.data();

    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &_config.input_assembly_info;
    pipeline_info.pViewportState = &_config.viewport_info;
    pipeline_info.pRasterizationState = &_config.rasterization_info;
    pipeline_info.pMultisampleState = &_config.multisample_info;
    pipeline_info.pColorBlendState = &_config.color_blend_info;
    pipeline_info.pDepthStencilState = &_config.depth_stencil_info;
    pipeline_info.pDynamicState = &_config.dynamic_state_info;

    pipeline_info.layout = _config.pipeline_layout;
    pipeline_info.renderPass = _config.render_pass;
    pipeline_info.subpass = _config.subpass;

    pipeline_info.basePipelineIndex = -1;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

    if(vkCreateGraphicsPipelines(
            _device.device(),
            VK_NULL_HANDLE,
            1,
            &pipeline_info,
            nullptr,
            &_graphics_pipeline) != VK_SUCCESS) {
      throw std::runtime_error("failed to create graphics pipeline");
    }
}

}