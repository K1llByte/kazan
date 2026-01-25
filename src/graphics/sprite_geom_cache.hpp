#pragma once

#include "core/log.hpp"
#include "core/string_hash.hpp"
#include "core/type.hpp"
#include "graphics/renderer.hpp"
#include "vk/buffer.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <unordered_map>

namespace kzn {

//! Vertex unit used for 2D rendering
struct Vertex2D {
    //! x and y components are positional, z is used for depth sorting.
    Vec3 position;
    //! Maps texture to the vertices
    Vec2 tex_coords;
};

struct SpriteGeometry {
    vk::VertexBuffer quad_vbo;
};

class SpriteGeometryCache {
public:
    // Ctor
    SpriteGeometryCache(Renderer& renderer) : m_renderer_ptr{&renderer} {}
    // Copy
    SpriteGeometryCache(const SpriteGeometryCache&) = delete;
    SpriteGeometryCache& operator=(const SpriteGeometryCache&) = delete;
    // Move
    SpriteGeometryCache(SpriteGeometryCache&&) = delete;
    SpriteGeometryCache& operator=(SpriteGeometryCache&&) = delete;
    // Dtor
    ~SpriteGeometryCache() = default;

    [[nodiscard]]
    std::shared_ptr<SpriteGeometry> find_or_create(
        float width,
        float height,
        bool centered = true
    ) {
        std::uint32_t hash = internal::Fnv1Params<std::uint32_t>::offset;

        constexpr std::size_t num_bytes = sizeof(float) * 2 + sizeof(bool);
        std::uint8_t bytes[num_bytes];

        std::memcpy(bytes, &width, sizeof(float));
        std::memcpy(bytes + sizeof(float), &height, sizeof(float));
        std::memcpy(bytes + 2 * sizeof(float), &centered, sizeof(bool));

        for (std::size_t i = 0; i < num_bytes; ++i) {
            hash *= internal::Fnv1Params<std::uint32_t>::prime;
            hash ^= bytes[i];
        }

        auto it = m_geometry.find(hash);
        if (it != m_geometry.end()) {
            return it->second;
        }
        else {
            // Create vertex buffer
            auto quad_vbo =
                vk::VertexBuffer(m_renderer_ptr->device(), sizeof(Vertex2D) * 4);

            // Upload quad vertex data to vertex buffer in the Gpu
            const Vec3 centered_offset = {width / 2.f, height / 2.f, 0.f};
            const std::array vertices{
                Vertex2D{Vec3{0, height, 0.f} - centered_offset, Vec2{0, 1}},
                Vertex2D{Vec3{0, 0, 0.f} - centered_offset, Vec2{0, 0}},
                Vertex2D{
                    Vec3{width, height, 0.f} - centered_offset, Vec2{1, 1}
                },
                Vertex2D{Vec3{width, 0, 0.f} - centered_offset, Vec2{1, 0}}
            };
            quad_vbo.upload(static_cast<const void*>(vertices.data()));

            // Create and insert SpriteGeometry
            auto inserted = m_geometry.emplace(
                hash, std::make_shared<SpriteGeometry>(std::move(quad_vbo))
            );
            return inserted.first->second;
        }
    }

    void clear() { m_geometry.clear(); }

private:
    Renderer* m_renderer_ptr;
    std::unordered_map<std::uint32_t, std::shared_ptr<SpriteGeometry>>
        m_geometry;
};

} // namespace kzn