#ifndef KZN_VK_PIPELINE_HPP
#define KZN_VK_PIPELINE_HPP

#include "vk/device.hpp"
#include "vk/render_pass.hpp"
#include "vk/cmd_buffers.hpp"
#include "vk/utils.hpp"

#include <string_view>

namespace kzn::vk
{
    struct PipelineConfig
    {
        VkPipelineViewportStateCreateInfo              viewport_info;
        std::vector<VkViewport>                        viewports;
        std::vector<VkRect2D>                          scissors;
        std::vector<VkVertexInputBindingDescription>   vtx_bindings;
        std::vector<VkVertexInputAttributeDescription> vtx_attributes;
        VkPipelineInputAssemblyStateCreateInfo         input_assembly_info;
        VkPipelineRasterizationStateCreateInfo         rasterization_info;
        VkPipelineMultisampleStateCreateInfo           multisample_info;
        VkPipelineColorBlendAttachmentState            color_blend_attachment;
        VkPipelineColorBlendStateCreateInfo            color_blend_info;
        VkPipelineDepthStencilStateCreateInfo          depth_stencil_info;
        std::vector<VkDynamicState>                    dynamic_state_enables;
        VkPipelineDynamicStateCreateInfo               dynamic_state_info;
        VkPipelineLayout                               pipeline_layout = VK_NULL_HANDLE;
        VkRenderPass                                   render_pass = VK_NULL_HANDLE;
        uint32_t                                       subpass = 0;
    };

    class PipelineConfigBuilder
    {
    public:
        PipelineConfigBuilder(
            VkPipelineLayout layout,
            RenderPass& render_pass);
        ~PipelineConfigBuilder() = default;

        // VertexInput
        template<typename ...Ts>
        PipelineConfigBuilder& set_vtx_input() noexcept;
        template<typename T>
        PipelineConfigBuilder& set_type_vtx_input() noexcept;
        // TODO: Non compile time version of this function
        // PipelineConfigBuilder& set_vtx_input(VertexInputDescription description) noexcept;

        // Input Assemply
        PipelineConfigBuilder& set_topology(VkPrimitiveTopology topology) noexcept;

        // Rasterization Stage
        PipelineConfigBuilder& set_polygon_mode(VkPolygonMode polygon_mode) noexcept;
        PipelineConfigBuilder& set_front_face(VkFrontFace font_face) noexcept;
        PipelineConfigBuilder& set_line_width(float line_width) noexcept;

        // Dynamic State Enables
        PipelineConfigBuilder& set_dynamic_states(const std::vector<VkDynamicState>& enables) noexcept;

        PipelineConfig build();

    private:
        PipelineConfig config;
    };

    class PipelineLayoutBuilder
    {
    public:
        PipelineLayoutBuilder(Device* device);

        // PipelineLayoutBuilder& add_descriptor_set(const DescriptorSet& set);
        PipelineLayoutBuilder& add_push_constant(uint32_t size, VkShaderStageFlags stages = VK_SHADER_STAGE_ALL_GRAPHICS);

        VkPipelineLayout build();

    private:
        Device*                            device = nullptr;
        // std::vector<VkDescriptorSetLayout> set_layouts;
        std::vector<VkPushConstantRange>   push_ranges;
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

        constexpr VkPipelineLayout layout() { return pipeline_layout; }

        void set_viewport(CommandBuffer& cmd_buffer, VkViewport viewport);
        void set_scissor(CommandBuffer& cmd_buffer, VkRect2D scissor);
        void bind(CommandBuffer& cmd_buffer);

    private:
        Device*          device = nullptr;
        VkPipeline       graphics_pipeline;
        VkPipelineLayout pipeline_layout;
        VkShaderModule   vert_shader_module;
        VkShaderModule   frag_shader_module;
        VkViewport       viewport;
    };


    template<typename ...Ts>
    PipelineConfigBuilder& PipelineConfigBuilder::set_vtx_input() noexcept
    {
        uint32_t stride = (sizeof(Ts) + ...);
        config.vtx_bindings = std::vector{vtx_binding(stride,/*binding*/ 0)};
        auto attribs = vtx_attributes<Ts...>(/*binding*/ 0);
        config.vtx_attributes = std::vector(attribs.begin(), attribs.end());
        return *this;
    }

    template<typename VertexType>
    PipelineConfigBuilder& PipelineConfigBuilder::set_type_vtx_input() noexcept
    {
        std::vector<VkVertexInputAttributeDescription> vertex_attributes;
        uint32_t i = 0;
        uint32_t offset = 0;
        boost::pfr::for_each_field(std::forward<VertexType>(VertexType{}), [&]<typename T>(T&) {

            VkFormat format;
            if constexpr (std::is_same_v<T, float>)
                format = VK_FORMAT_R32_SFLOAT;
            else if constexpr (std::is_same_v<T, glm::vec2>)
                format = VK_FORMAT_R32G32_SFLOAT;
            else if constexpr (std::is_same_v<T, glm::vec3>)
                format = VK_FORMAT_R32G32B32_SFLOAT;
            else if constexpr (std::is_same_v<T, glm::vec4>)
                format = VK_FORMAT_R32G32B32A32_SFLOAT;
            else if constexpr (std::is_same_v<T, glm::ivec2>)
                format = VK_FORMAT_R64_SFLOAT;
            else
                // FIXME:
                throw "Invalid vertex input attribute";

            vertex_attributes.push_back(vtx_attribute(0, i, format, offset));
            offset += sizeof(T);
            ++i;
        });

        config.vtx_bindings = std::vector{vtx_binding(sizeof(VertexType), 0)};
        config.vtx_attributes = std::move(vertex_attributes);
        return *this;
    }
} // namespace kzn::vk


#endif // KZN_VK_PIPELINE_HPP