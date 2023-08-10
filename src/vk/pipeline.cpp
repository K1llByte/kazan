#include "pipeline.hpp"

#include "core/log.hpp"
#include "vk/error.hpp"

#include <fstream>

namespace kzn::vk {

PipelineConfig::PipelineConfig() {
    m_input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    m_input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    m_input_assembly_info.primitiveRestartEnable = VK_FALSE;

    // VkViewport viewport{};
    // viewport.x = 0.0f;
    // viewport.y = 0.0f;
    // viewport.width = (float) viewport_extent.width;
    // viewport.height = (float) viewport_extent.height;
    // viewport.minDepth = 0.0f;
    // viewport.maxDepth = 1.0f;
    // m_viewports.push_back(viewport);        
    // VkRect2D scissor{};
    // scissor.offset = {0, 0};
    // scissor.extent = viewport_extent;
    // m_scissors.push_back(scissor);
    
    // 
    m_viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    // m_viewport_info.viewportCount = m_viewports.size();
    // m_viewport_info.pViewports = m_viewports.data();
    m_viewport_info.viewportCount = 1;
    m_viewport_info.pViewports = nullptr;
    // m_viewport_info.scissorCount = m_scissors.size();
    // m_viewport_info.pScissors = m_scissors.data();
    m_viewport_info.scissorCount = 1;
    m_viewport_info.pScissors = nullptr;

    m_rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    m_rasterization_info.depthClampEnable = VK_FALSE;
    m_rasterization_info.rasterizerDiscardEnable = VK_FALSE;
    m_rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
    m_rasterization_info.lineWidth = 1.0f;
    m_rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT; // VK_CULL_MODE_NONE;
    m_rasterization_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; //VK_FRONT_FACE_CLOCKWISE;
    m_rasterization_info.depthBiasEnable = VK_FALSE;
    m_rasterization_info.depthBiasConstantFactor = 0.0f;  // Optional
    m_rasterization_info.depthBiasClamp = 0.0f;           // Optional
    m_rasterization_info.depthBiasSlopeFactor = 0.0f;     // Optional

    m_multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    m_multisample_info.sampleShadingEnable = VK_FALSE;
    m_multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    m_multisample_info.minSampleShading = 1.0f;           // Optional
    m_multisample_info.pSampleMask = nullptr;             // Optional
    m_multisample_info.alphaToCoverageEnable = VK_FALSE;  // Optional
    m_multisample_info.alphaToOneEnable = VK_FALSE;       // Optional

    m_color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
                                            | VK_COLOR_COMPONENT_G_BIT
                                            | VK_COLOR_COMPONENT_B_BIT
                                            | VK_COLOR_COMPONENT_A_BIT;
    m_color_blend_attachment.blendEnable = VK_FALSE;
    m_color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    m_color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    m_color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
    m_color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    m_color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    m_color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

    m_color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    m_color_blend_info.logicOpEnable = VK_FALSE;
    m_color_blend_info.logicOp = VK_LOGIC_OP_COPY;  // Optional
    m_color_blend_info.attachmentCount = 1;
    m_color_blend_info.pAttachments = &m_color_blend_attachment;
    m_color_blend_info.blendConstants[0] = 0.0f;  // Optional
    m_color_blend_info.blendConstants[1] = 0.0f;  // Optional
    m_color_blend_info.blendConstants[2] = 0.0f;  // Optional
    m_color_blend_info.blendConstants[3] = 0.0f;  // Optional

    m_depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    m_depth_stencil_info.depthTestEnable = VK_TRUE;
    m_depth_stencil_info.depthWriteEnable = VK_TRUE;
    m_depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
    m_depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
    m_depth_stencil_info.minDepthBounds = 0.0f;  // Optional
    m_depth_stencil_info.maxDepthBounds = 1.0f;  // Optional
    m_depth_stencil_info.stencilTestEnable = VK_FALSE;
    m_depth_stencil_info.front = {};  // Optional
    m_depth_stencil_info.back = {};   // Optional

    // By default VK_DYNAMIC_STATE_VIEWPORT is enabled
    m_dynamic_state_enables = { VK_DYNAMIC_STATE_VIEWPORT };
    // m_dynamic_state_enables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    m_dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    m_dynamic_state_info.pDynamicStates = m_dynamic_state_enables.data();
    m_dynamic_state_info.dynamicStateCount =
        static_cast<uint32_t>(m_dynamic_state_enables.size());
    m_dynamic_state_info.flags = 0;
}


ShaderModule::ShaderModule(Device& device, std::string_view file_path)
    : m_device{device}
{
    // 1. Read code from file and store it in a vector
    std::ifstream file{file_path.data(), std::ios::ate | std::ios::binary};
    if(!file.is_open()) {
        Log::error("Failed to open file '{}'", file_path);
        throw 1; // TODO: Error handling
        //throw FileError();
    }

    const size_t file_size = static_cast<size_t>(file.tellg());
    std::vector<char> code(file_size);
    file.seekg(0);
    file.read(code.data(), file_size);
    file.close();

    // 2. Create shader module from the code
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    auto result = vkCreateShaderModule(m_device.vk_device(), &create_info, nullptr, &m_shader_module);
    VK_CHECK_MSG(result, "Failed to load shader module!");
}

ShaderModule::~ShaderModule() {
    vkDestroyShaderModule(m_device.vk_device(), m_shader_module, nullptr);
}


} // namespace kzn::vk