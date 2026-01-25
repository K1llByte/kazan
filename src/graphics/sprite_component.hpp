#pragma once

#include "graphics/sprite_geom_cache.hpp"
#include "graphics/sprite_material.hpp"
#include "vk/uniform.hpp"

#include <memory>

namespace kzn {

// FIXME: Shouldn't load statically
static inline auto s_default_material =
    std::make_shared<SpriteMaterial>("assets/textures/debug.png");

class SpriteComponent {
public:
    struct SpriteUniformData {
        glsl::Vec4 slice;
        glsl::Vec4 overlap_color;
    };

    SpriteComponent(
        float width = 1.f,
        float height = 1.f,
        std::shared_ptr<SpriteMaterial> material_ptr = s_default_material,
        bool centered = true
    )
        : m_width{width}
        , m_height{height}
        , m_centered{centered}
        , m_material_ptr{material_ptr}
    {
    }

    ~SpriteComponent() = default;

    [[nodiscard]]
    constexpr float width() const {
        return m_width;
    }

    [[nodiscard]]
    constexpr float height() const {
        return m_height;
    }

    [[nodiscard]]
    std::shared_ptr<SpriteGeometry> geometry() {
        return m_geometry_ptr;
    }

    [[nodiscard]]
    std::shared_ptr<SpriteMaterial> material() {
        return m_material_ptr;
    }

    void create_geometry(SpriteGeometryCache& geometry_cache) {
        // Will create and upload vertex data to the GPU
        m_geometry_ptr =
            geometry_cache.find_or_create(m_width, m_height, m_centered);
    }

private:
    float m_width;
    float m_height;
    bool m_centered;
    std::shared_ptr<SpriteGeometry> m_geometry_ptr = nullptr;
    std::shared_ptr<SpriteMaterial> m_material_ptr = nullptr;
};

} // namespace kzn