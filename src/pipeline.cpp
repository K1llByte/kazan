#include "pipeline.hpp"

#include <fstream>

namespace kzn
{

VkShaderModule* Pipeline::create_shader_module(const std::string& file_path)
{
    // 1. Read code from file and store it in a vector
    std::ifstream file{file_path, std::ios::ate | std::ios::binary};

    if(!file.is_open())
    {
        throw std::runtime_error("failed to open file: " + file_path);
    }

    size_t file_size = static_cast<size_t>(file.tellg());
    std::vector<char> code(file_size);

    file.seekg(0);
    file.read(code.data(), file_size);

    file.close();

    // 2. Create shader module from the code
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule* shader_module;
    if(vkCreateShaderModule(_device.device(), &create_info, nullptr, shader_module) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module");
    }

    return shader_module;
}

}
