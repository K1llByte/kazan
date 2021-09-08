#include "pipeline.hpp"

#include "model.hpp"

#include <fstream>

namespace kzn
{

Pipeline::Pipeline(
        Device& device,
        const std::string& vert_shader_path,
        const std::string& frag_shader_path,
        const PipelineConfigInfo& config_info)
    : _device{device}
{
    create_graphics_pipeline(vert_shader_path, frag_shader_path, config_info);
}


Pipeline::~Pipeline()
{
    vkDestroyShaderModule(_device.device(), _vert_shader_module, nullptr);
    vkDestroyShaderModule(_device.device(), _frag_shader_module, nullptr);
    vkDestroyPipeline(_device.device(), _graphics_pipeline, nullptr);
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
    const PipelineConfigInfo& config_info)
{
    if(config_info.pipeline_layout == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Cannot create graphics pipeline: no 'pipeline_layout' provided in 'config_info'");
    }

    if(config_info.render_pass == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Cannot create graphics pipeline: no 'render_pass' provided in 'config_info'");
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
    pipeline_info.pInputAssemblyState = &config_info.input_assembly_info;
    pipeline_info.pViewportState = &config_info.viewport_info;
    pipeline_info.pRasterizationState = &config_info.rasterization_info;
    pipeline_info.pMultisampleState = &config_info.multisample_info;
    pipeline_info.pColorBlendState = &config_info.color_blend_info;
    pipeline_info.pDepthStencilState = &config_info.depth_stencil_info;
    pipeline_info.pDynamicState = &config_info.dynamic_state_info;

    pipeline_info.layout = config_info.pipeline_layout;
    pipeline_info.renderPass = config_info.render_pass;
    pipeline_info.subpass = config_info.subpass;

    pipeline_info.basePipelineIndex = -1;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(
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
