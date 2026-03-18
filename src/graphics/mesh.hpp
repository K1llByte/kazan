#pragma once

#include "math/types.hpp"
#include "resources/resources.hpp"
#include "vk/buffer.hpp"
#include "vk/device.hpp"

#include <filesystem>
#include <memory>
#include <vector>

namespace kzn {

struct Vertex3D {
    Vec3 position;
    Vec3 normal;
    Vec3 color = Vec3{1,1,1};
};

struct MeshData {
    std::vector<Vertex3D> vertices;
    std::vector<std::uint32_t> indices;
    // VertexLayout layout;

    [[nodiscard]]
    static std::shared_ptr<MeshData> load(const std::filesystem::path& path);
};

class Mesh {
public:
    // Ctor
    Mesh(vk::Device& device, const MeshData& mesh_data);
    // Copy
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    // Move
    Mesh(Mesh&&) = default;
    Mesh& operator=(Mesh&&) = default;
    // Dtor
    ~Mesh() = default;

    [[nodiscard]]
    const vk::VertexBuffer& vtx_buffer() const {
        return m_vtx_buffer;
    }
    [[nodiscard]]
    const vk::IndexBuffer& idx_buffer() const {
        return m_idx_buffer;
    }
    
    [[nodiscard]]
    std::size_t vtx_count() const {
        return m_vtx_count;
    }
    [[nodiscard]]
    std::size_t idx_count() const {
        return m_idx_count;
    }

private:
    std::size_t m_vtx_count;
    std::size_t m_idx_count;
    vk::VertexBuffer m_vtx_buffer;
    vk::IndexBuffer m_idx_buffer;
};

class MeshComponent {
public:
    // Ctor
    MeshComponent(vk::Device &device, const std::string_view mesh_path)
        : m_mesh(device, *g_resources.load<MeshData>(mesh_path)) {}
    // Copy
    MeshComponent(const MeshComponent&) = delete;
    MeshComponent& operator=(const MeshComponent&) = delete;
    // Move
    MeshComponent(MeshComponent&&) = default;
    MeshComponent& operator=(MeshComponent&&) = default;
    // Dtor
    ~MeshComponent() = default;

    [[nodiscard]]
    const Mesh& mesh() const {
        return m_mesh;
    }

private:
    Mesh m_mesh;
};

} // namespace kzn