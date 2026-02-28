#include "pipeline_builder.hpp"
#include "core/assert.hpp"
#include "core/flat_map.hpp"
#include "core/log.hpp"
#include "spirv_reflect.h"
#include "vk/pipeline.hpp"
#include "vk/shader_code.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <ranges>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace kzn::vk {

PipelineBuilder::PipelineBuilder(
    VkRenderPass render_pass
)
    : m_render_pass{render_pass}
{
    // Default pipeline state values
    
    m_input_assembly_info = {};
    m_input_assembly_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    m_input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    m_input_assembly_info.primitiveRestartEnable = VK_FALSE;

    m_viewport_info = {};
    m_viewport_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    // m_viewport_info.viewportCount = m_viewports.size();
    // m_viewport_info.pViewports = m_viewports.data();
    m_viewport_info.viewportCount = 1;
    // m_viewport_info.pViewports = nullptr;
    // m_viewport_info.scissorCount = m_scissors.size();
    // m_viewport_info.pScissors = m_scissors.data();
    m_viewport_info.scissorCount = 1;
    // m_viewport_info.pScissors = nullptr;

    m_rasterization_info = {};
    m_rasterization_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    m_rasterization_info.depthClampEnable = VK_FALSE;
    m_rasterization_info.rasterizerDiscardEnable = VK_FALSE;
    m_rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
    m_rasterization_info.lineWidth = 1.0f;
    m_rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT; // VK_CULL_MODE_NONE;
    m_rasterization_info.frontFace =
        VK_FRONT_FACE_CLOCKWISE; // VK_FRONT_FACE_COUNTER_CLOCKWISE;
    m_rasterization_info.depthBiasEnable = VK_FALSE;
    m_rasterization_info.depthBiasConstantFactor = 0.0f;
    m_rasterization_info.depthBiasClamp = 0.0f;
    m_rasterization_info.depthBiasSlopeFactor = 0.0f;

    m_multisample_info = {};
    m_multisample_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    m_multisample_info.sampleShadingEnable = VK_FALSE;
    m_multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    m_multisample_info.minSampleShading = 1.0f;          // Optional
    m_multisample_info.pSampleMask = nullptr;            // Optional
    m_multisample_info.alphaToCoverageEnable = VK_FALSE; // Optional
    m_multisample_info.alphaToOneEnable = VK_FALSE;      // Optional

    m_color_blend_attachment = {};
    m_color_blend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    m_color_blend_attachment.blendEnable = VK_TRUE;
    m_color_blend_attachment.srcColorBlendFactor =
        VK_BLEND_FACTOR_SRC_ALPHA; // Optional
    m_color_blend_attachment.dstColorBlendFactor =
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;                 // Optional
    m_color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    m_color_blend_attachment.srcAlphaBlendFactor =
        VK_BLEND_FACTOR_ONE; // Optional
    m_color_blend_attachment.dstAlphaBlendFactor =
        VK_BLEND_FACTOR_ZERO;                                // Optional
    m_color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    m_color_blend_info = {};
    m_color_blend_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    m_color_blend_info.logicOpEnable = VK_FALSE;
    m_color_blend_info.logicOp = VK_LOGIC_OP_COPY; // Optional
    m_color_blend_info.attachmentCount = 1;
    m_color_blend_info.pAttachments = &m_color_blend_attachment;
    m_color_blend_info.blendConstants[0] = 0.0f; // Optional
    m_color_blend_info.blendConstants[1] = 0.0f; // Optional
    m_color_blend_info.blendConstants[2] = 0.0f; // Optional
    m_color_blend_info.blendConstants[3] = 0.0f; // Optional

    m_depth_stencil_info = {};
    m_depth_stencil_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    m_depth_stencil_info.depthTestEnable = VK_TRUE;
    m_depth_stencil_info.depthWriteEnable = VK_TRUE;
    m_depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
    m_depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
    m_depth_stencil_info.minDepthBounds = 0.0f; // Optional
    m_depth_stencil_info.maxDepthBounds = 1.0f; // Optional
    m_depth_stencil_info.stencilTestEnable = VK_FALSE;
    m_depth_stencil_info.front = {}; // Optional
    m_depth_stencil_info.back = {};  // Optional

    // FIXME: For now, by default VK_DYNAMIC_STATE_VIEWPORT,
    // VK_DYNAMIC_STATE_SCISSOR and VK_DYNAMIC_STATE_LINE_WIDTH are enabled.
    m_dynamic_state_info = {};
    m_dynamic_state_enables = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };
    m_dynamic_state_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    m_dynamic_state_info.pDynamicStates = m_dynamic_state_enables.data();
    m_dynamic_state_info.dynamicStateCount =
        static_cast<uint32_t>(m_dynamic_state_enables.size());
    m_dynamic_state_info.flags = 0;
}

PipelineBuilder& PipelineBuilder::set_vertex_stage(std::shared_ptr<ShaderCode> shader) {
    m_vertex_stage = std::move(shader);
    return *this;
}

PipelineBuilder& PipelineBuilder::set_tess_control_stage(std::shared_ptr<ShaderCode> shader) {
    m_tess_control_stage = std::move(shader);
    return *this;
}

PipelineBuilder& PipelineBuilder::set_tess_evaluation_stage(std::shared_ptr<ShaderCode> shader) {
    m_tess_evaluation_stage = std::move(shader);
    return *this;
}

PipelineBuilder& PipelineBuilder::set_geometry_stage(std::shared_ptr<ShaderCode> shader) {
    m_geometry_stage = std::move(shader);
    return *this;
}

PipelineBuilder& PipelineBuilder::set_fragment_stage(std::shared_ptr<ShaderCode> shader) {
    m_fragment_stage = std::move(shader);
    return *this;
}

PipelineBuilder& PipelineBuilder::set_topology(VkPrimitiveTopology topology) {
    m_input_assembly_info.topology = topology;
    return *this;
}

PipelineBuilder& PipelineBuilder::set_polygon_mode(VkPolygonMode polygon_mode) {
    m_rasterization_info.polygonMode = polygon_mode;
    return *this;
}

PipelineBuilder& PipelineBuilder::set_front_face(VkFrontFace font_face) {
    m_rasterization_info.frontFace = font_face;
    return *this;
}

PipelineBuilder& PipelineBuilder::set_line_width(float line_width) {
    m_rasterization_info.lineWidth = line_width;
    return *this;
}

struct ShaderReflection {
    FlatMap<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> descriptor_sets;
    std::vector<VkPushConstantRange> push_constants;
};

ShaderReflection reflect_shader(
    const std::span<ShaderCode*> shader_stages
) {
    ShaderReflection reflection = {};

    for(std::size_t i = 0; i < shader_stages.size(); ++i) {
        // Create reflected shader module
        SpvReflectShaderModule module;
        auto result = spvReflectCreateShaderModule(
            shader_stages[i]->bytecode.size(),
            shader_stages[i]->bytecode.data(),
            &module
        );
        KZN_ASSERT_MSG(
            result == SPV_REFLECT_RESULT_SUCCESS,
            "Could not reflect shader module"
        );

        // Merge push constants
        for(std::size_t i = 0; i < module.push_constant_block_count; ++i) {
            auto* block_ptr = &module.push_constant_blocks[i];
            // Try find a push constant block with same offset and size.
            auto it = std::ranges::find_if(
                reflection.push_constants,
                [block_ptr](const auto& pc_range) {
                    return block_ptr->offset == pc_range.offset
                        && block_ptr->size == pc_range.size;
                }
            );            
            // If found, then merge shader stage flags.
            // Otherwise add to vector.
            if(it != reflection.push_constants.end()) {
                it->stageFlags |= module.shader_stage;
            }
            else {
                reflection.push_constants.push_back(VkPushConstantRange{
                    .stageFlags = module.shader_stage,
                    .offset = block_ptr->offset,
                    .size = block_ptr->size,
                });
            }
        }

        // Merge descriptor sets
        for(std::size_t s = 0; s < module.descriptor_set_count; ++s) {
            auto* dset_ptr = &module.descriptor_sets[s];
            for(std::size_t b = 0; b < dset_ptr->binding_count; ++b) {
                auto* binding_ptr = dset_ptr->bindings[b];

                auto vk_binding = VkDescriptorSetLayoutBinding{
                    .binding = binding_ptr->binding,
                    // SpvReflectDescriptorType and VkDescriptorType
                    // should be numerically compatible.
                    .descriptorType = static_cast<VkDescriptorType>(
                        binding_ptr->descriptor_type
                    ),
                    .descriptorCount = binding_ptr->count,
                    .stageFlags = module.shader_stage,
                    .pImmutableSamplers = nullptr,
                };

                // Try emplace a new set
                auto [it, inserted] = reflection.descriptor_sets.emplace(
                    dset_ptr->set,
                    std::vector{vk_binding}
                );
                // If already exists, try merging the binding, if the binding
                // already exists, merge stage flags.
                if(!inserted) {
                    auto binding_it = std::ranges::find_if(
                        it->second,
                        [binding_ptr](const auto& vk_binding) {
                            return vk_binding.binding == binding_ptr->binding;
                        }
                    );

                    if(binding_it != it->second.end()) {
                        // FIXME: Instead of assert, these checks should be
                        // runtime error.
                        KZN_ASSERT(
                            static_cast<VkDescriptorType>(binding_ptr->descriptor_type)
                                == binding_it->descriptorType
                        );
                        KZN_ASSERT(
                            binding_ptr->count == binding_it->descriptorCount
                        );

                        binding_it->stageFlags |= module.shader_stage;
                    }
                    else {
                        it->second.push_back(vk_binding);
                    }
                }
            }
        }

        // When no longer needed, destroy reflection module
        spvReflectDestroyShaderModule(&module);
    }

    return reflection;
}


static VkShaderModule create_shader_module(
    Device& device,
    const ShaderCode& shader_code
) {
    // Create shader module from the code
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = shader_code.bytecode.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(shader_code.bytecode.data());

    VkShaderModule shader_module;
    auto result =
        vkCreateShaderModule(device, &create_info, nullptr, &shader_module);
    VK_CHECK_MSG(result, "Failed to load shader module!");
    return shader_module;
}

Pipeline PipelineBuilder::build(Device& device) {

    // Partition array to have null stages at last
    // No need to take ownership in this auxiliary function.
    auto shader_stages = std::array{
        m_vertex_stage.get(),
        m_tess_control_stage.get(),
        m_tess_evaluation_stage.get(),
        m_geometry_stage.get(),
        m_fragment_stage.get()
    };
    const auto it = std::ranges::partition(
        shader_stages,
        [](ShaderCode* ptr) { return ptr != nullptr; }
    ).begin();

    const std::size_t valid_stages_count = std::distance(shader_stages.begin(), it);
    KZN_ASSERT_MSG(valid_stages_count != 0, "No shader stages provided");
    const std::span valid_stages = {shader_stages.data(), valid_stages_count};

    ///////////////////////////////////////////////////////////////////////////
    // Create shader modules
    ///////////////////////////////////////////////////////////////////////////

    std::array<VkShaderModule, 5> vk_shader_modules;
    std::array<VkPipelineShaderStageCreateInfo, 5> vk_shader_create_infos;

    std::size_t stage_idx = 0;
    const auto create_mod_and_info = [&](const ShaderCode* shader_stage_ptr, VkShaderStageFlagBits stage_flag) {
        if(shader_stage_ptr != nullptr) {
            vk_shader_modules[stage_idx] = create_shader_module(device, *shader_stage_ptr);
            vk_shader_create_infos[stage_idx].sType =
                VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vk_shader_create_infos[stage_idx].stage = stage_flag;
            vk_shader_create_infos[stage_idx].module = vk_shader_modules[stage_idx];
            vk_shader_create_infos[stage_idx].pName = "main";
            vk_shader_create_infos[stage_idx].flags = 0;
            vk_shader_create_infos[stage_idx].pNext = nullptr;
            vk_shader_create_infos[stage_idx].pSpecializationInfo = nullptr;
            stage_idx += 1;
        }
    };
    
    create_mod_and_info(m_vertex_stage.get(), VK_SHADER_STAGE_VERTEX_BIT);
    create_mod_and_info(m_tess_control_stage.get(), VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
    create_mod_and_info(m_tess_evaluation_stage.get(), VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
    create_mod_and_info(m_geometry_stage.get(), VK_SHADER_STAGE_GEOMETRY_BIT);
    create_mod_and_info(m_fragment_stage.get(), VK_SHADER_STAGE_FRAGMENT_BIT);

    ///////////////////////////////////////////////////////////////////////////
    // Create descriptor set layouts
    ///////////////////////////////////////////////////////////////////////////

    auto reflection = reflect_shader(valid_stages);
    PipelineLayout layout{
        .push_constants = std::move(reflection.push_constants),
    };
    layout.descriptor_sets.reserve(reflection.descriptor_sets.size());
    
    for(auto& [set, bindings] : reflection.descriptor_sets) {
        VkDescriptorSetLayout dset_layout;
        VkDescriptorSetLayoutCreateInfo dset_layout_create_info{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(bindings.size()),
            .pBindings = bindings.data(),
        };
        auto result = vkCreateDescriptorSetLayout(
            device, &dset_layout_create_info, nullptr, &dset_layout
        );
        VK_CHECK_MSG(result, "Failed to create descriptor set layout");

        layout.descriptor_sets.push_back(dset_layout);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Create pipeline layout
    ///////////////////////////////////////////////////////////////////////////
    
    VkPipelineLayoutCreateInfo pipeline_layout_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = static_cast<uint32_t>(layout.descriptor_sets.size()),
        .pSetLayouts = layout.descriptor_sets.data(),
        .pushConstantRangeCount = static_cast<uint32_t>(
            layout.push_constants.size()
        ),
        .pPushConstantRanges = layout.push_constants.data(),
    };
    VkPipelineLayout vk_pipeline_layout;
    auto result = vkCreatePipelineLayout(
        device, &pipeline_layout_info, nullptr, &vk_pipeline_layout
    );
    VK_CHECK_MSG(result, "Failed to create pipeline layout!");

    ///////////////////////////////////////////////////////////////////////////
    // Setup vertex input state create info
    ///////////////////////////////////////////////////////////////////////////

    // Setup vertex input description
    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(m_vertex_attributes.size());
    vertex_input_info.vertexBindingDescriptionCount =
        static_cast<uint32_t>(m_vertex_bindings.size());
    vertex_input_info.pVertexAttributeDescriptions =
        m_vertex_attributes.data();
    vertex_input_info.pVertexBindingDescriptions =
        m_vertex_bindings.data();

    ///////////////////////////////////////////////////////////////////////////
    // Create pipeline
    ///////////////////////////////////////////////////////////////////////////
    
    VkGraphicsPipelineCreateInfo pipeline_create_info{};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.stageCount = valid_stages_count;
    pipeline_create_info.pStages = vk_shader_create_infos.data();
    pipeline_create_info.pVertexInputState = &vertex_input_info;
    pipeline_create_info.pInputAssemblyState = &m_input_assembly_info;
    pipeline_create_info.pViewportState = &m_viewport_info;
    pipeline_create_info.pRasterizationState = &m_rasterization_info;
    pipeline_create_info.pMultisampleState = &m_multisample_info;
    pipeline_create_info.pColorBlendState = &m_color_blend_info;
    pipeline_create_info.pDepthStencilState = &m_depth_stencil_info;
    pipeline_create_info.pDynamicState = &m_dynamic_state_info;

    pipeline_create_info.layout = vk_pipeline_layout;
    pipeline_create_info.renderPass = m_render_pass;
    pipeline_create_info.subpass = m_subpass;

    pipeline_create_info.basePipelineIndex = -1;
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    
    auto pipeline = Pipeline(device, pipeline_create_info);

    ///////////////////////////////////////////////////////////////////////////
    // Destroy shader modules and dset layouts
    ///////////////////////////////////////////////////////////////////////////

    for (std::size_t i = 0; i < valid_stages_count; ++i) {
        vkDestroyShaderModule(device, vk_shader_modules[i], nullptr);
    }
    for(auto dset_layout : layout.descriptor_sets) {
        vkDestroyDescriptorSetLayout(device, dset_layout, nullptr);
    }

    return pipeline;
}

} // namespace kzn::vk
