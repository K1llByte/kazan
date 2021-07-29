#ifndef PIPELINE
#define PIPELINE

#include "device.hpp"

#include <vector>
#include <string>

namespace kzn
{

struct PipelineConfigInfo {
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
    VkPipelineRasterizationStateCreateInfo rasterization_info;
    VkPipelineMultisampleStateCreateInfo multisample_info;
    VkPipelineColorBlendAttachmentState color_blend_attachment;
    VkPipelineColorBlendStateCreateInfo color_blend_info;
    VkPipelineDepthStencilStateCreateInfo depth_stencil_info;
    VkPipelineLayout pipeline_layout = nullptr;
    VkRenderPass render_pass = nullptr;
    uint32_t subpass = 0;
};

class Pipeline
{
private:
    Device& m_device;
    VkPipeline m_graphics_pipeline;
    VkShaderModule m_vert_shader_module;
    VkShaderModule m_frag_shader_module;

private:
    static std::vector<char> read_file(const std::string& file_path);
    void create_pipeline(
        const std::string& vert_path,
        const std::string& frag_path,
        const PipelineConfigInfo& config);
    void create_shader_module(const std::vector<char>& code, VkShaderModule* shader_module);

public:
    Pipeline(
        Device& device,
        const std::string& vert_path,
        const std::string& frag_path,
        const PipelineConfigInfo& config);
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    void operator=(const Pipeline&) = delete;

    static PipelineConfigInfo default_config(uint32_t width, uint32_t height);
};

}

#endif // PIPELINE