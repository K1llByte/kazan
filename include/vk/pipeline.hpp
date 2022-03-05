#ifndef KZN_VK_PIPELINE_HPP
#define KZN_VK_PIPELINE_HPP

#include "vk/device.hpp"

namespace kzn::vk
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
    public:
        PipelineConfigBuilder(
            VkPipelineLayout layout = VK_NULL_HANDLE,
            VkRenderPass render_pass = VK_NULL_HANDLE);
        ~PipelineConfigBuilder() = default;

        PipelineConfigBuilder& set_layout(VkPipelineLayout layout);
        PipelineConfigBuilder& set_render_pass(VkRenderPass render_pass);

        // Input Assemply
        PipelineConfigBuilder& set_topology(VkPrimitiveTopology topology);

        // Rasterization Stage
        PipelineConfigBuilder& set_polygon_mode(VkPolygonMode polygon_mode);

        PipelineConfig build();

    private:
        PipelineConfig config;
    };

    class Pipeline
    {
    public:
        Pipeline(
            Device& device,
            const std::string& vert_shader_path,
            const std::string& frag_shader_path,
            const PipelineConfig& config_info);
        ~Pipeline();

        void bind(VkCommandBuffer command_buffer);

    private:
        VkShaderModule create_shader_module(const std::string& file_path);

        void create_graphics_pipeline(
            const std::string& vert_path,
            const std::string& frag_path,
            const PipelineConfig& config_info);

    private:
        Device&        device;
        VkPipeline     graphics_pipeline;
        VkShaderModule vert_shader_module;
        VkShaderModule frag_shader_module;
    };
} // namespace kzn::vk

#endif // KZN_VK_PIPELINE_HPP