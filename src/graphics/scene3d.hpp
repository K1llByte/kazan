#pragma once

#include "mesh.hpp"

#include <memory>
#include <vector>

namespace kzn {

struct Scene3DData {
    std::vector<MeshData> meshes;

    [[nodiscard]]
    static std::shared_ptr<Scene3DData> load(const std::filesystem::path& path);
};

} // namespace kzn