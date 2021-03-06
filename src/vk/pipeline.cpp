#include "vk/pipeline.hpp"

#include "vk/error.hpp"
#include "vk/utils.hpp"

#include <fstream>

namespace kzn::vk
{
    VkShaderModule create_shader_module(Device& device, const std::string_view& file_path)
    {
        // 1. Read code from file and store it in a vector //
        std::ifstream file{file_path.data(), std::ios::ate | std::ios::binary};
        if(!file.is_open())
        {
            Log::error("Failed to open file '{}'", file_path);
            throw FileError();
        }
        size_t file_size = static_cast<size_t>(file.tellg());
        std::vector<char> code(file_size);
        file.seekg(0);
        file.read(code.data(), file_size);
        file.close();

        // 2. Create shader module from the code //
        VkShaderModuleCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = code.size();
        create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());
        VkShaderModule shader_module;
        if(vkCreateShaderModule(device.vk_device(), &create_info, nullptr, &shader_module) != VK_SUCCESS)
        {
            throw ShaderModError();
        }
        return shader_module;
    }

    PipelineConfigBuilder::PipelineConfigBuilder(
        VkPipelineLayout layout,
        RenderPass& render_pass)
        : config{}
    {
        config.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        config.input_assembly_info.primitiveRestartEnable = VK_FALSE;

        // VkViewport viewport{};
        // viewport.x = 0.0f;
        // viewport.y = 0.0f;
        // viewport.width = (float) viewport_extent.width;
        // viewport.height = (float) viewport_extent.height;
        // viewport.minDepth = 0.0f;
        // viewport.maxDepth = 1.0f;
        // config.viewports.push_back(viewport);        
        // VkRect2D scissor{};
        // scissor.offset = {0, 0};
        // scissor.extent = viewport_extent;
        // config.scissors.push_back(scissor);
        
        // 
        config.viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        // config.viewport_info.viewportCount = config.viewports.size();
        // config.viewport_info.pViewports = config.viewports.data();
        config.viewport_info.viewportCount = 1;
        config.viewport_info.pViewports = nullptr;
        // config.viewport_info.scissorCount = config.scissors.size();
        // config.viewport_info.pScissors = config.scissors.data();
        config.viewport_info.scissorCount = 1;
        config.viewport_info.pScissors = nullptr;

        config.rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        config.rasterization_info.depthClampEnable = VK_FALSE;
        config.rasterization_info.rasterizerDiscardEnable = VK_FALSE;
        config.rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
        config.rasterization_info.lineWidth = 1.0f;
        config.rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT; // VK_CULL_MODE_NONE;
        config.rasterization_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; //VK_FRONT_FACE_CLOCKWISE;
        config.rasterization_info.depthBiasEnable = VK_FALSE;
        config.rasterization_info.depthBiasConstantFactor = 0.0f;  // Optional
        config.rasterization_info.depthBiasClamp = 0.0f;           // Optional
        config.rasterization_info.depthBiasSlopeFactor = 0.0f;     // Optional

        config.multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        config.multisample_info.sampleShadingEnable = VK_FALSE;
        config.multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        config.multisample_info.minSampleShading = 1.0f;           // Optional
        config.multisample_info.pSampleMask = nullptr;             // Optional
        config.multisample_info.alphaToCoverageEnable = VK_FALSE;  // Optional
        config.multisample_info.alphaToOneEnable = VK_FALSE;       // Optional

        config.color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
                                                          | VK_COLOR_COMPONENT_G_BIT
                                                          | VK_COLOR_COMPONENT_B_BIT
                                                          | VK_COLOR_COMPONENT_A_BIT;
        config.color_blend_attachment.blendEnable = VK_FALSE;
        config.color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        config.color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        config.color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
        config.color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        config.color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        config.color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

        config.color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        config.color_blend_info.logicOpEnable = VK_FALSE;
        config.color_blend_info.logicOp = VK_LOGIC_OP_COPY;  // Optional
        config.color_blend_info.attachmentCount = 1;
        config.color_blend_info.pAttachments = &config.color_blend_attachment;
        config.color_blend_info.blendConstants[0] = 0.0f;  // Optional
        config.color_blend_info.blendConstants[1] = 0.0f;  // Optional
        config.color_blend_info.blendConstants[2] = 0.0f;  // Optional
        config.color_blend_info.blendConstants[3] = 0.0f;  // Optional

        config.depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        config.depth_stencil_info.depthTestEnable = VK_TRUE;
        config.depth_stencil_info.depthWriteEnable = VK_TRUE;
        config.depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
        config.depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
        config.depth_stencil_info.minDepthBounds = 0.0f;  // Optional
        config.depth_stencil_info.maxDepthBounds = 1.0f;  // Optional
        config.depth_stencil_info.stencilTestEnable = VK_FALSE;
        config.depth_stencil_info.front = {};  // Optional
        config.depth_stencil_info.back = {};   // Optional

        // By default VK_DYNAMIC_STATE_VIEWPORT is enabled
        config.dynamic_state_enables = { VK_DYNAMIC_STATE_VIEWPORT };
        // config.dynamic_state_enables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        config.dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        config.dynamic_state_info.pDynamicStates = config.dynamic_state_enables.data();
        config.dynamic_state_info.dynamicStateCount =
            static_cast<uint32_t>(config.dynamic_state_enables.size());
        config.dynamic_state_info.flags = 0;

        config.pipeline_layout = layout;
        config.render_pass = render_pass.vk_render_pass();
    }

    PipelineConfigBuilder& PipelineConfigBuilder::set_topology(VkPrimitiveTopology topology) noexcept
    {
        config.input_assembly_info.topology = topology;
        return *this;
    }

    PipelineConfigBuilder& PipelineConfigBuilder::set_line_width(float line_width) noexcept
    {
        config.rasterization_info.lineWidth = line_width;
        return *this;
    }

    PipelineConfigBuilder& PipelineConfigBuilder::set_dynamic_states(const std::vector<VkDynamicState>& enables) noexcept
    {
        config.dynamic_state_enables = enables;
        config.dynamic_state_info.pDynamicStates = config.dynamic_state_enables.data();
        config.dynamic_state_info.dynamicStateCount = 
            static_cast<uint32_t>(config.dynamic_state_enables.size());
        return *this;
    }

    PipelineConfigBuilder& PipelineConfigBuilder::set_polygon_mode(VkPolygonMode polygon_mode) noexcept
    {
        config.rasterization_info.polygonMode = polygon_mode;
        return *this;
    }

    PipelineConfigBuilder& PipelineConfigBuilder::set_front_face(VkFrontFace front_face) noexcept
    {
        config.rasterization_info.frontFace = front_face;
        return *this;
    }


    PipelineConfig PipelineConfigBuilder::build()
    {
        if(config.render_pass == VK_NULL_HANDLE
            || config.pipeline_layout == VK_NULL_HANDLE)
        {
            // TODO: Since these are required parameters this exception shouldn't exist
            throw std::runtime_error("'VkPipelineLayout' and 'VkRenderPass' must be set");
        }
        return std::move(config);
    }

    PipelineLayoutBuilder::PipelineLayoutBuilder(Device* device)
        : device(device) {}

    PipelineLayoutBuilder& PipelineLayoutBuilder::add_push_constant(uint32_t size, VkShaderStageFlags stages)
    {
        push_ranges.emplace_back(stages, 0, size);
        return *this;
    }

    VkPipelineLayout PipelineLayoutBuilder::build()
    {
        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        // pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
        // pipeline_layout_info.pSetLayouts = set_layouts.data();

        pipeline_layout_info.setLayoutCount = 0;
        pipeline_layout_info.pSetLayouts = nullptr;
        pipeline_layout_info.pushConstantRangeCount = static_cast<uint32_t>(push_ranges.size());
        pipeline_layout_info.pPushConstantRanges = push_ranges.data();

        VkPipelineLayout pipeline_layout;
        auto result = vkCreatePipelineLayout(device->vk_device(), &pipeline_layout_info, nullptr, &pipeline_layout);
        VK_CHECK_MSG(result, "Failed to create pipeline layout!");
        return pipeline_layout;
    }

    Pipeline::Pipeline(
        Device* device,
        const std::string_view& vert_shader_path,
        const std::string_view& frag_shader_path,
        const PipelineConfig& config)
        : device{device}
    {
        vert_shader_module = create_shader_module(*device, vert_shader_path);
        frag_shader_module = create_shader_module(*device, frag_shader_path);
        pipeline_layout = config.pipeline_layout;

        VkPipelineShaderStageCreateInfo shader_stages[2];
        // Vertex Shader
        shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shader_stages[0].module = vert_shader_module;
        shader_stages[0].pName = "main";
        shader_stages[0].flags = 0;
        shader_stages[0].pNext = nullptr;
        shader_stages[0].pSpecializationInfo = nullptr;
        // Fragment Shader
        shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shader_stages[1].module = frag_shader_module;
        shader_stages[1].pName = "main";
        shader_stages[1].flags = 0;
        shader_stages[1].pNext = nullptr;
        shader_stages[1].pSpecializationInfo = nullptr;

        // Setup vertex input description
        VkPipelineVertexInputStateCreateInfo vertex_input_info{};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(config.vtx_attributes.size());
        vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(config.vtx_bindings.size());
        vertex_input_info.pVertexAttributeDescriptions = config.vtx_attributes.data();
        vertex_input_info.pVertexBindingDescriptions = config.vtx_bindings.data();

        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount = 2;
        pipeline_info.pStages = shader_stages;
        pipeline_info.pVertexInputState = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &config.input_assembly_info;
        pipeline_info.pViewportState = &config.viewport_info;
        pipeline_info.pRasterizationState = &config.rasterization_info;
        pipeline_info.pMultisampleState = &config.multisample_info;
        pipeline_info.pColorBlendState = &config.color_blend_info;
        pipeline_info.pDepthStencilState = &config.depth_stencil_info;
        pipeline_info.pDynamicState = &config.dynamic_state_info;

        pipeline_info.layout = config.pipeline_layout;
        pipeline_info.renderPass = config.render_pass;
        pipeline_info.subpass = config.subpass;

        pipeline_info.basePipelineIndex = -1;
        pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

        auto result = vkCreateGraphicsPipelines(
                device->vk_device(),
                VK_NULL_HANDLE,
                1,
                &pipeline_info,
                nullptr,
                &graphics_pipeline);
        VK_CHECK_MSG(result, "Failed to create graphics pipeline!");
        Log::debug("Pipeline created");
    }

    Pipeline::~Pipeline()
    {
        vkDestroyShaderModule(device->vk_device(), vert_shader_module, nullptr);
        vkDestroyShaderModule(device->vk_device(), frag_shader_module, nullptr);
        vkDestroyPipelineLayout(device->vk_device(), pipeline_layout, nullptr);
        vkDestroyPipeline(device->vk_device(), graphics_pipeline, nullptr);
        Log::debug("Pipeline destroyed");
    }

    void Pipeline::set_viewport(CommandBuffer& cmd_buffer, VkViewport _viewport)
    {
        vkCmdSetViewport(
            cmd_buffer.vk_command_buffer(),
            0,
            1,
            &_viewport
        );
    }

    void Pipeline::set_scissor(CommandBuffer& cmd_buffer, VkRect2D scissor)
    {
        vkCmdSetScissor(
            cmd_buffer.vk_command_buffer(),
            0,
            1,
            &scissor
        );
    }

    void Pipeline::bind(CommandBuffer& cmd_buffer)
    {
        vkCmdBindPipeline(
            cmd_buffer.vk_command_buffer(),
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            graphics_pipeline
        );
    }
}