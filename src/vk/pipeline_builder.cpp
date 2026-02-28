#include "pipeline_builder.hpp"
#include "core/assert.hpp"
#include "spirv_reflect.h"
#include "vk/shader_code.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <ranges>

namespace kzn::vk {

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

struct DSetLayoutBinding {
    VkDescriptorSetLayoutBinding binding;
};

struct ShaderReflection {
    std::vector<SpvReflectShaderModule> modules;
    std::vector<SpvReflectDescriptorSet*> descriptor_sets;
    std::vector<VkPushConstantRange> push_constants;
};

static ShaderReflection reflect_shader(
    std::array<ShaderCode*, 5> shader_stages
) {
    // Partition array to have null stages at last
    const auto it = std::ranges::partition(
        shader_stages,
        [](ShaderCode* ptr) { return ptr != nullptr; }
    ).begin();
    const std::size_t valid_stages_count = std::distance(shader_stages.begin(), it);
    KZN_ASSERT(valid_stages_count != 0);
    const std::span valid_stages = {shader_stages.data(), valid_stages_count};

    ShaderReflection reflection = {};
    reflection.modules.resize(valid_stages_count);

    for(std::size_t i = 0; i < valid_stages_count; ++i) {
        // Create reflected shader module
        auto& module = reflection.modules[i];
        auto result = spvReflectCreateShaderModule(
            valid_stages[i]->bytecode.size(),
            valid_stages[i]->bytecode.data(),
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
                [block_ptr](auto pc_range) {
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
        for(std::size_t i = 0; i < module.descriptor_set_count; ++i) {
            // TODO:
        }
    }

    return reflection;
}

// Pipeline PipelineBuilder::build() {
//     auto reflection = ShaderReflection{};
//     // TODO: Reflect each shader stage
    
//     // No need to take ownership in this auxiliary function.
//     reflect_shader_stage(std::array{
//         m_vertex_stage.get(),
//         m_tess_control_stage.get(),
//         m_tess_evaluation_stage.get(),
//         m_geometry_stage.get(),
//         m_fragment_stage.get()
//     });
//     // TODO: Merge stages reflected dsets and push constants
//     // TODO: Create pipeline layout

//     // // Create pipeline layout
//     // VkPipelineLayoutCreateInfo pipeline_layout_info{
//     //     .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
//     //     .setLayoutCount = ,
//     //     .pSetLayouts = ,
//     //     .pushConstantRangeCount = ,
//     //     .pPushConstantRanges = ,
//     // };

//     // auto result = vkCreatePipelineLayout(
//     //     m_device, &pipeline_layout_info, nullptr, &m_pipeline_layout
//     // );
//     // VK_CHECK_MSG(result, "Failed to create pipeline layout!");
//     // return ;
// }

} // namespace kzn::vk
