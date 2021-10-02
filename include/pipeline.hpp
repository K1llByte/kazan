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
private:

public:

    PipelineLayoutBuilder(Renderer& renderer);
    ~PipelineLayoutBuilder() = default;

    template<typename T>
    [[nodiscard]] UniformBuffer<T> create_uniform_buffer(VkShaderStageFlags stages = VK_SHADER_STAGE_ALL_GRAPHICS);

    template<typename T>
    [[nodiscard]] PushConstant<T> create_push_constant(VkShaderStageFlags stages = VK_SHADER_STAGE_ALL_GRAPHICS);

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

}

#endif // PIPELINE_H
