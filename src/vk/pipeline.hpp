#pragma once

#include "render_pass.hpp"
#include "utils.hpp"
#include "vk/uniform.hpp"

#include <string_view>
#include <vulkan/vulkan_core.h>

namespace kzn::vk {

struct PipelineLayout {
    std::vector<VkPushConstantRange> push_constants;
    std::vector<VkDescriptorSetLayout> descriptor_sets;
};

template<glsl::UniformBlock T>
VkPushConstantRange push_constant_range(
    VkShaderStageFlags stages = VK_SHADER_STAGE_ALL_GRAPHICS
) {
    return VkPushConstantRange{
        .stageFlags = stages,
        .offset = 0,
        .size = sizeof(T),
    };
}

class PipelineConfig {
    friend class Pipeline;

public:
    // Ctor
    PipelineConfig(const vk::RenderPass& render_pass);
    // Copy
    PipelineConfig(const PipelineConfig&) = delete;
    PipelineConfig& operator=(const PipelineConfig&) = delete;
    // Move
    PipelineConfig(PipelineConfig&&) = default;
    PipelineConfig& operator=(PipelineConfig&&) = default;
    // Dtor
    ~PipelineConfig() = default;

    // Pipeline layout
    PipelineConfig& set_layout(PipelineLayout&& layout) {
        m_pipeline_layout = std::move(layout);
        return *this;
    }

    // VertexInput
    template<typename... Ts>
    PipelineConfig& set_vertex_input();

    // template<typename T>
    // PipelineConfig& set_type_vertex_input() noexcept;

    //! Input Assemply >
    PipelineConfig& set_topology(VkPrimitiveTopology topology) {
        m_input_assembly_info.topology = topology;
        return *this;
    }

    // // Rasterization Stage
    PipelineConfig& set_polygon_mode(VkPolygonMode polygon_mode) {
        m_rasterization_info.polygonMode = polygon_mode;
        return *this;
    }

    PipelineConfig& set_front_face(VkFrontFace font_face) {
        m_rasterization_info.frontFace = font_face;
        return *this;
    }

    PipelineConfig& set_line_width(float line_width) {
        m_rasterization_info.lineWidth = line_width;
        return *this;
    }

    // // Dynamic State Enables
    // PipelineConfig& set_dynamic_states(const std::vector<VkDynamicState>&
    // enables) noexcept;

private:
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

struct PipelineStages {
    std::string_view vertex;
    std::string_view tess_control;
    std::string_view tess_evaluation;
    std::string_view geometry;
    std::string_view fragment;
};

class Pipeline {
public:
    // Ctor
    Pipeline(
        Device& device,
        const PipelineStages& stages,
        const PipelineConfig& config
    );
    // Copy
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    // Move
    Pipeline(Pipeline&&) = delete;
    Pipeline& operator=(Pipeline&&) = delete;
    // Dtor
    ~Pipeline();

    [[nodiscard]]
    VkPipelineLayout layout() const {
        return m_pipeline_layout;
    }

    void bind(vk::CommandBuffer& cmd_buffer);

private:
    Device& m_device;
    VkPipeline m_vk_pipeline;
    VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
    std::vector<VkShaderModule> m_shader_modules;
};

//////////////////////////////////////////////////////////////
//                      Implementation                      //
//////////////////////////////////////////////////////////////

template<typename... Ts>
PipelineConfig& PipelineConfig::set_vertex_input() {
    uint32_t stride = (sizeof(Ts) + ...);
    m_vertex_bindings = std::vector{vertex_binding(stride, /* binding */ 0)};
    auto attribs = vertex_attributes<Ts...>(/* binding */ 0);
    m_vertex_attributes = std::vector(attribs.begin(), attribs.end());
    return *this;
}

} // namespace kzn::vk
