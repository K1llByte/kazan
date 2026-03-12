#pragma once

#include "math/types.hpp"
#include "vk/buffer.hpp"
#include "vk/device.hpp"
#include <filesystem>
#include <memory>
#include <vector>

namespace kzn {

struct Vertex3D {
    Vec3 position;
    Vec3 color = Vec3{1,1,1};
};

struct MeshData {
    std::vector<Vertex3D> vertices;
    std::vector<uint32_t> indices;
    // VertexLayout layout;

    [[nodiscard]]
    static std::shared_ptr<MeshData> load(const std::filesystem::path& path);
};

class Mesh {
public:
    // Ctor
    Mesh(vk::Device& device, const std::vector<Vertex3D>& vertices);
    // Copy
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    // Move
    Mesh(Mesh&&) = delete;
    Mesh& operator=(Mesh&&) = delete;
    // Dtor
    ~Mesh() = default;

    [[nodiscard]]
    const vk::VertexBuffer& vtx_buffer() const {
        return m_vtx_buffer;
    }
    
    [[nodiscard]]
    std::size_t vtx_count() const {
        return m_vtx_count;
    }

private:
    std::size_t m_vtx_count;
    vk::VertexBuffer m_vtx_buffer;
};

} // namespace kzn