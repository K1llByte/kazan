#pragma once

#include "vk/pipeline.hpp"

#include <memory>

namespace kzn::vk {

class PipelineBuilder {
public:
    PipelineBuilder() = default;
    PipelineBuilder(PipelineBuilder&&) = default;
    PipelineBuilder(const PipelineBuilder&) = delete;
    ~PipelineBuilder() = default;

    // Pipeline Stages
    PipelineBuilder& set_vertex_stage(std::shared_ptr<ShaderCode> shader);
    PipelineBuilder& set_tess_control_stage(std::shared_ptr<ShaderCode> shader);
    PipelineBuilder& set_tess_evaluation_stage(std::shared_ptr<ShaderCode> shader);
    PipelineBuilder& set_geometry_stage(std::shared_ptr<ShaderCode> shader);
    PipelineBuilder& set_fragment_stage(std::shared_ptr<ShaderCode> shader);

    // // Vertex Input
    // template<typename... Ts>
    // PipelineConfig& set_vertex_input();

    // Input Assemply
    PipelineBuilder& set_topology(VkPrimitiveTopology topology);

    // Rasterization Stage
    PipelineBuilder& set_polygon_mode(VkPolygonMode polygon_mode);
    PipelineBuilder& set_front_face(VkFrontFace font_face);
    PipelineBuilder& set_line_width(float line_width);

    [[nodiscard]]
    Pipeline build();

private:
    std::shared_ptr<ShaderCode> m_vertex_stage = nullptr;
    std::shared_ptr<ShaderCode> m_tess_control_stage = nullptr;
    std::shared_ptr<ShaderCode> m_tess_evaluation_stage = nullptr;
    std::shared_ptr<ShaderCode> m_geometry_stage = nullptr;
    std::shared_ptr<ShaderCode> m_fragment_stage = nullptr;

    VkPipelineViewportStateCreateInfo m_viewport_info;
    std::vector<VkViewport> m_viewports;
    std::vector<VkRect2D> m_scissors;
    std::vector<VkVertexInputBindingDescription> m_vertex_bindings;
    std::vector<VkVertexInputAttributeDescription> m_vertex_attributes;
    VkPipelineInputAssemblyStateCreateInfo m_input_assembly_info;
    VkPipelineRasterizationStateCreateInfo m_rasterization_info;
    VkPipelineMultisampleStateCreateInfo m_multisample_info;
    VkPipelineColorBlendAttachmentState m_color_blend_attachment;
    VkPipelineColorBlendStateCreateInfo m_color_blend_info;
    VkPipelineDepthStencilStateCreateInfo m_depth_stencil_info;
    std::vector<VkDynamicState> m_dynamic_state_enables;
    VkPipelineDynamicStateCreateInfo m_dynamic_state_info;
    PipelineLayout m_pipeline_layout;
    VkRenderPass m_render_pass = VK_NULL_HANDLE;
    uint32_t m_subpass = 0;
};

} // namespace kzn::vk