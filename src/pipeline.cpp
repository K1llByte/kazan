#include "pipeline.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace kzn
{

std::vector<char> Pipeline::read_file(const std::string& file_path)
{
    // TODO: validate file size limit

    // Open file at the end to compute size
    std::ifstream file{file_path, std::ios::ate | std::ios::binary};

    if(!file.is_open())
    {
        throw std::runtime_error("failed to open file: " + file_path);
    }

    const size_t file_size = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);

    file.close();
    return buffer;
}

void Pipeline::create_pipeline(
    const std::string& vert_path,
    const std::string& frag_path,
    const PipelineConfigInfo& config)
{
    std::vector<char> vert_bin = read_file(vert_path);
    std::vector<char> frag_bin = read_file(frag_path);

    std::cout << vert_bin.size() << '\n';
    std::cout << frag_bin.size() << '\n';
}

void Pipeline::create_shader_module(const std::vector<char>& code, VkShaderModule* shader_module)
{
    VkShaderModuleCreateInfo create_info;
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if(vkCreateShaderModule(m_device.device(), &create_info, nullptr, shader_module) != VK_SUCCESS)
        throw std::runtime_error("failde to create shader module");
}

Pipeline::Pipeline(
    Device device,
    const std::string& vert_path,
    const std::string& frag_path,
    const PipelineConfigInfo& config)
    : m_device{device}
{
    create_pipeline(vert_path, frag_path, config);
}

PipelineConfigInfo Pipeline::default_config(uint32_t width, uint32_t height)
{
    PipelineConfigInfo config;

    return config;
}


}