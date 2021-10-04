#ifndef PIPELINE_H
#define PIPELINE_H

#include "device.hpp"
#include "renderer.hpp"

namespace kzn
{

struct PipelineConfig
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
    VkPipelineLayout                       pipeline_layout = VK_NULL_HANDLE;
    VkRenderPass                           render_pass = VK_NULL_HANDLE;
    uint32_t                               subpass = 0;
};


class PipelineConfigBuilder
{
private:

    PipelineConfig _config;

public:

    PipelineConfigBuilder(
        VkPipelineLayout layout = VK_NULL_HANDLE,
        VkRenderPass render_pass = VK_NULL_HANDLE);
    ~PipelineConfigBuilder() = default;

    PipelineConfigBuilder& set_layout(VkPipelineLayout layout);
    PipelineConfigBuilder& set_render_pass(VkRenderPass render_pass);

    // Input Assemply
    PipelineConfigBuilder& set_topology(VkPrimitiveTopology topology);

    PipelineConfig&& build();
};


class PipelineLayoutBuilder
{
public:

    Device*                            device;
    std::vector<VkDescriptorSetLayout> set_layouts;
    std::vector<VkPushConstantRange>   push_ranges;

public:

    PipelineLayoutBuilder& add_descriptor_set(const DescriptorSet& set);

    template<typename T>
    PipelineLayoutBuilder& add_push_constant(const PushConstant<T>& push);

    VkPipelineLayout build();
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
        const PipelineConfig& config_info);
    ~Pipeline();

    void bind(VkCommandBuffer command_buffer);

    static void default_pipeline_config_info(PipelineConfig& config_info);

private:

    VkShaderModule create_shader_module(const std::string& file_path);

    void create_graphics_pipeline(
        const std::string& vert_path,
        const std::string& frag_path,
        const PipelineConfig& config_info);

};

////////////////////////////////////////////////////////////////

PipelineLayoutBuilder& PipelineLayoutBuilder::add_descriptor_set(const DescriptorSet& set)
{
    set_layouts.push_back(set.descriptor_set_layout);
    return *this;
}


template<typename T>
PipelineLayoutBuilder& PipelineLayoutBuilder::add_push_constant(const PushConstant<T>& push)
{
    push_ranges.push_back(
        VkPushConstantRange{
            .stageFlags = push.stages,
            .offset = 0,
            .size = sizeof(T)
        }
    );
    return *this;
}

}

#endif // PIPELINE_H
