#ifndef PIPELINE_H
#define PIPELINE_H

#include "device.hpp"

namespace kzn
{

struct PipelineConfigInfo
{
    VkPipelineViewportStateCreateInfo      viewport_info;
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
    VkPipelineRasterizationStateCreateInfo rasterization_info;
    VkPipelineMultisampleStateCreateInfo   multisample_info;
    VkPipelineColorBlendAttachmentState    color_blend_attachment;
    VkPipelineColorBlendStateCreateInfo    color_blend_info;
    VkPipelineDepthStencilStateCreateInfo  depth_stencil_info;
    std::vector<VkDynamicState>            dynamic_state_enables;
    VkPipelineDynamicStateCreateInfo       dynamic_state_info;
    VkPipelineLayout                       pipeline_layout = nullptr;
    VkRenderPass                           render_pass = nullptr;
    uint32_t                               subpass = 0;
};

class Pipeline
{
private:

    Device&        _device;
    VkPipeline     _graphics_pipeline;
    VkShaderModule _vert_shader_module;
    VkShaderModule _frag_shader_module;

public:

    Pipeline(
        Device& device,
        const std::string& vert_shader_path,
        const std::string& frag_shader_path,
        const PipelineConfigInfo& config_info);
    ~Pipeline();

    void bind(VkCommandBuffer command_buffer);

    static void default_pipeline_config_info(PipelineConfigInfo& config_info);

private:

    VkShaderModule create_shader_module(const std::string& file_path);

    void create_graphics_pipeline(
        const std::string& vert_path,
        const std::string& frag_path,
        const PipelineConfigInfo& config_info);

};

}

#endif // PIPELINE_H
