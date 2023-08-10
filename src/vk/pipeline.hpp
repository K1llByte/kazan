#pragma once

#include "device.hpp"

#include <string_view>

namespace kzn::vk {

class PipelineConfig {
public:
    // Ctor
    PipelineConfig();
    // Copy
    PipelineConfig(const PipelineConfig&) = delete;
    PipelineConfig& operator=(const PipelineConfig&) = delete;
    // Move
    PipelineConfig(PipelineConfig&&) = default;
    PipelineConfig& operator=(PipelineConfig&&) = default;
    // Dtor
    ~PipelineConfig() = default;

    // // VertexInput
    // template<typename ...Ts>
    // PipelineConfigBuilder& set_vtx_input() noexcept;
    // template<typename T>
    // PipelineConfigBuilder& set_type_vtx_input() noexcept;
    // // TODO: Non compile time version of this function
    // // PipelineConfigBuilder& set_vtx_input(VertexInputDescription description) noexcept;

    // // Input Assemply
    // PipelineConfigBuilder& set_topology(VkPrimitiveTopology topology) noexcept;

    // // Rasterization Stage
    // PipelineConfigBuilder& set_polygon_mode(VkPolygonMode polygon_mode) noexcept;
    // PipelineConfigBuilder& set_front_face(VkFrontFace font_face) noexcept;
    // PipelineConfigBuilder& set_line_width(float line_width) noexcept;

    // // Dynamic State Enables
    // PipelineConfigBuilder& set_dynamic_states(const std::vector<VkDynamicState>& enables) noexcept;

private:
    VkPipelineViewportStateCreateInfo              m_viewport_info;
    std::vector<VkViewport>                        m_viewports;
    std::vector<VkRect2D>                          m_scissors;
    std::vector<VkVertexInputBindingDescription>   m_vtx_bindings;
    std::vector<VkVertexInputAttributeDescription> m_vtx_attributes;
    VkPipelineInputAssemblyStateCreateInfo         m_input_assembly_info;
    VkPipelineRasterizationStateCreateInfo         m_rasterization_info;
    VkPipelineMultisampleStateCreateInfo           m_multisample_info;
    VkPipelineColorBlendAttachmentState            m_color_blend_attachment;
    VkPipelineColorBlendStateCreateInfo            m_color_blend_info;
    VkPipelineDepthStencilStateCreateInfo          m_depth_stencil_info;
    std::vector<VkDynamicState>                    m_dynamic_state_enables;
    VkPipelineDynamicStateCreateInfo               m_dynamic_state_info;
    //PipelineLayout                                 m_pipeline_layout;
    //VkRenderPass                                   m_render_pass = VK_NULL_HANDLE;
    //uint32_t                                       m_subpass = 0;
};

class ShaderModule {
public:
    // Ctor
    ShaderModule(Device& device, std::string_view filename);
    // Copy
    ShaderModule(const ShaderModule&) = delete;
    ShaderModule& operator=(const ShaderModule&) = delete;
    // Move
    ShaderModule(ShaderModule&&) = default;
    ShaderModule& operator=(ShaderModule&&) = default;
    // Dtor
    ~ShaderModule() = default;

private:
    VkShaderModule m_shader_module;
};

} // namespace kzn::vk
