#ifndef KZN_VK_PIPELINE_HPP
#define KZN_VK_PIPELINE_HPP

#include "vk/device.hpp"
#include "vk/render_pass.hpp"
#include "vk/cmd_buffers.hpp"

#include <string_view>

namespace kzn::vk
{
    struct PipelineConfig
    {
        VkPipelineViewportStateCreateInfo      viewport_info;
        std::vector<VkViewport>                viewports;
        std::vector<VkRect2D>                  scissors;
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
            VkPipelineLayout layout,
            RenderPass& render_pass);
        ~PipelineConfigBuilder() = default;

        // Input Assemply
        PipelineConfigBuilder& set_topology(VkPrimitiveTopology topology);

        // Rasterization Stage
        PipelineConfigBuilder& set_polygon_mode(VkPolygonMode polygon_mode);
        PipelineConfigBuilder& set_line_width(float line_width);

        // Dynamic State Enables
        PipelineConfigBuilder& set_dynamic_states(const std::vector<VkDynamicState>& enables);

        PipelineConfig build();

    private:
        PipelineConfig config;
    };

    class PipelineLayoutBuilder
    {
    public:
        PipelineLayoutBuilder(Device* device);

        // PipelineLayoutBuilder& add_descriptor_set(const DescriptorSet& set);
        // template<typename T>
        // PipelineLayoutBuilder& add_push_constant(const PushConstant<T>& push);

        VkPipelineLayout build() noexcept;

    private:
        Device* device = nullptr;
        // std::vector<VkDescriptorSetLayout> set_layouts;
        // std::vector<VkPushConstantRange>   push_ranges;
    };

    class Pipeline
    {
    public:
        // TODO: Make a Builder for Pipeline too when
        // more shaders are used
        Pipeline(
            Device* device,
            const std::string_view& vert_shader_path,
            const std::string_view& frag_shader_path,
            const PipelineConfig& config_info);
        ~Pipeline();

        void set_viewport(CommandBuffer& cmd_buffer, VkViewport viewport);
        void set_scissor(CommandBuffer& cmd_buffer, VkRect2D scissor);
        void bind(CommandBuffer cmd_buffer);

    private:
        Device*          device = nullptr;
        VkPipeline       graphics_pipeline;
        VkPipelineLayout pipeline_layout;
        VkShaderModule   vert_shader_module;
        VkShaderModule   frag_shader_module;
        VkViewport       viewport;
    };
} // namespace kzn::vk

#endif // KZN_VK_PIPELINE_HPP