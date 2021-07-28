#ifndef PIPELINE
#define PIPELINE

#include "device.hpp"

#include <vector>
#include <string>

namespace kzn
{

struct PipelineConfigInfo
{

};

class Pipeline
{
private:
    Device& m_device;
    VkPipeline graphics_pipeline;
    VkShaderModule vert_shader_module;
    VkShaderModule frag_shader_module;

private:
    static std::vector<char> read_file(const std::string& file_path);
    void create_pipeline(
        const std::string& vert_path,
        const std::string& frag_path,
        const PipelineConfigInfo& config);
    void create_shader_module(const std::vector<char>& code, VkShaderModule* shader_module);

public:
    Pipeline(
        Device device,
        const std::string& vert_path,
        const std::string& frag_path,
        const PipelineConfigInfo& config);
    ~Pipeline() {}

    Pipeline(const Pipeline&) = delete;
    void operator=(const Pipeline&) = delete;

    static PipelineConfigInfo default_config(uint32_t width, uint32_t height);
};

}

#endif // PIPELINE