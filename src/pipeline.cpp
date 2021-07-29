#include "pipeline.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
// #include <array>

namespace kzn
{

std::vector<char> Pipeline::read_file(const std::string& file_path)
{
    // TODO: validate file size limit

    // Open file at the end to compute size
    std::ifstream file{file_path, std::ios::ate | std::ios::binary};

    if(!file.is_open())
    {
        throw std::runtime_error("failed to open file: " + file_path);
    }

    const size_t file_size = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);

    file.close();
    return buffer;
}

void Pipeline::create_pipeline(
    const std::string& vert_path,
    const std::string& frag_path,
    const PipelineConfigInfo& config)
{
    std::vector<char> vert_bin = read_file(vert_path);
    std::vector<char> frag_bin = read_file(frag_path);

    // std::cout << vert_bin.size() << '\n';
    // std::cout << frag_bin.size() << '\n';

    create_shader_module(vert_bin, &m_vert_shader_module);
    create_shader_module(frag_bin, &m_frag_shader_module);

    VkPipelineShaderStageCreateInfo shader_stages[2];

    shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader_stages[0].module = m_vert_shader_module;
    shader_stages[0].pName = "main";
    shader_stages[0].flags = 0;
    shader_stages[0].pNext = nullptr;
    shader_stages[0].pSpecializationInfo = nullptr;

    shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_stages[1].module = m_frag_shader_module;
    shader_stages[1].pName = "main";
    shader_stages[1].flags = 0;
    shader_stages[1].pNext = nullptr;
    shader_stages[1].pSpecializationInfo = nullptr;

    VkPipelineVertexInputStateCreateInfo vert_input_info;
    vert_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vert_input_info.vertexAttributeDescriptionCount = 0;
    vert_input_info.vertexBindingDescriptionCount = 0;
    vert_input_info.pVertexAttributeDescriptions = nullptr;
    vert_input_info.pVertexBindingDescriptions = nullptr;

    VkGraphicsPipelineCreateInfo pipeline_info;
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;
    pipeline_info.pVertexInputState = &vert_input_info;
    pipeline_info.pInputAssemblyState = &config.input_assembly_info;
    pipeline_info.pViewportState = &config.viewport_info;
    pipeline_info.pRasterizationState = &config.rasterization_info;
    pipeline_info.pMultisampleState = &config.multisample_info  ;
    pipeline_info.pColorBlendState = &config.color_blend_info;
    pipeline_info.pDepthStencilState = &config.depth_stencil_info;
    pipeline_info.pDepthStencilState = nullptr;

    pipeline_info.layout = config.pipeline_layout;
    pipeline_info.subpass = config.subpass;

    pipeline_info.basePipelineIndex = -1;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

    if(vkCreateGraphicsPipelines(m_device.device(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_graphics_pipeline) != VK_SUCCESS)
        throw std::runtime_error("failed to create graphics pipeline");
}

void Pipeline::create_shader_module(const std::vector<char>& code, VkShaderModule* shader_module)
{
    VkShaderModuleCreateInfo create_info;
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if(vkCreateShaderModule(m_device.device(), &create_info, nullptr, shader_module) != VK_SUCCESS)
        throw std::runtime_error("failde to create shader module");
}

Pipeline::Pipeline(
    Device& device,
    const std::string& vert_path,
    const std::string& frag_path,
    const PipelineConfigInfo& config)
    : m_device{device}
{
    create_pipeline(vert_path, frag_path, config);
}

Pipeline::~Pipeline()
{
    vkDestroyShaderModule(m_device.device(), m_vert_shader_module, nullptr);
    vkDestroyShaderModule(m_device.device(), m_frag_shader_module, nullptr);
    vkDestroyPipeline(m_device.device(), m_graphics_pipeline, nullptr);
}

PipelineConfigInfo Pipeline::default_config(uint32_t width, uint32_t height)
{
    PipelineConfigInfo config;

    config.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    config.input_assembly_info.primitiveRestartEnable = VK_FALSE;

    config.viewport.x = 0.0f;
    config.viewport.y = 0.0f;
    config.viewport.width = static_cast<float>(width);
    config.viewport.height = static_cast<float>(height);
    config.viewport.minDepth = 0.0f;
    config.viewport.maxDepth = 1.0f;

    config.scissor.offset = {0, 0};
    config.scissor.extent = {width, height};

    config.viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    config.viewport_info.viewportCount = 1;
    config.viewport_info.pViewports = &config.viewport;
    config.viewport_info.scissorCount = 1;
    config.viewport_info.pScissors = &config.scissor;

    config.rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    config.rasterization_info.depthClampEnable = VK_FALSE;
    config.rasterization_info.rasterizerDiscardEnable = VK_FALSE;
    config.rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
    config.rasterization_info.lineWidth = 1.0f;
    config.rasterization_info.cullMode = VK_CULL_MODE_NONE;
    config.rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    config.rasterization_info.depthBiasEnable = VK_FALSE;
    config.rasterization_info.depthBiasConstantFactor = 0.0f;
    config.rasterization_info.depthBiasClamp = 0.0f;
    config.rasterization_info.depthBiasSlopeFactor = 0.0f;
 
    config.multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    config.multisample_info.sampleShadingEnable = VK_FALSE;
    config.multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    config.multisample_info.minSampleShading = 1.0f;           // Optional
    config.multisample_info.pSampleMask = nullptr;             // Optional
    config.multisample_info.alphaToCoverageEnable = VK_FALSE;  // Optional
    config.multisample_info.alphaToOneEnable = VK_FALSE;       // Optional
 
    config.color_blend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
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
    
    return config;
}


}