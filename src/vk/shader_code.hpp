#pragma once

#include <filesystem>
#include <memory>
#include <vector>

namespace kzn::vk {

struct ShaderCode {
    std::vector<char> bytecode;
    
    [[nodiscard]]
    static std::shared_ptr<ShaderCode> load(const std::filesystem::path& path);
};

} // namespace kzn::vk