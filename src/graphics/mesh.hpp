#pragma once

#include "math/types.hpp"
#include <filesystem>
#include <memory>
#include <vector>

namespace kzn {

struct MeshData {
    std::vector<Vec3> vertices;
    std::vector<uint32_t> indices;
    // VertexLayout layout;

    [[nodiscard]]
    static std::shared_ptr<MeshData> load(const std::filesystem::path& path);
};

// class Mesh {
// public:

// private:
// };

} // namespace kzn