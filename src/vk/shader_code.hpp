#pragma once

#include "core/log.hpp"
#include "resources/resource.hpp"

#include <fmt/format.h>

#include <filesystem>
#include <fstream>
#include <vector>

namespace kzn {

struct ShaderCode {
    std::vector<char> bytecode;
    
    [[nodiscard]]
    static std::shared_ptr<ShaderCode> load(const std::filesystem::path& path) {

        // Read code from file and store it in a vector
        auto file = std::ifstream{path, std::ios::ate | std::ios::binary};
        if(!file.is_open()) {
            auto error_msg = fmt::format("Failed to open file '{}'", path.c_str());
            Log::error(error_msg);
            throw LoadingError{error_msg};
        }

        const size_t file_size = static_cast<size_t>(file.tellg());
        auto bytecode = std::vector<char>(file_size);
        file.seekg(0);
        file.read(bytecode.data(), file_size);
        file.close();

        return std::make_shared<ShaderCode>(bytecode);
    }
};

} // namespace kzn