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
    // , m_texture_ptr(Resources::find_or_load<Texture>(file_path))
    // , m_sprite_dset{Renderer::dset_allocator().allocate(
    //       Renderer::dset_layout_cache().create_layout({
    //           vk::sampler_binding(0),
    //           vk::uniform_binding(1),
    //       })
    //   )}
    // , m_image(Renderer::device(), m_texture_ptr->extent())
    // , m_sprite_ubo(Renderer::device(), sizeof(SpriteUniformData)) {
    {
        // set_slice({0, 0}, {1, 1});
        // upload_sprite_data();
        // // Upload texture data to gpu image memory
        // m_image.upload(m_texture_ptr->data());
        // // Update descriptor set bindings
        // m_sprite_dset.update({m_image.info(), m_sprite_ubo.info()});
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

    // [[nodiscard]]
    // vk::DescriptorSet& sprite_dset() {
    //     return m_sprite_dset;
    // }

    // void set_overlap_color(Vec4 overlap_color) {
    //     m_overlap_color = overlap_color;
    // }

    // void set_slice(Vec2 offset, Vec2 size) {
    //     m_slice_offset = offset;
    //     m_slice_size = size;
    // }

    // FIXME: This function should only be called once per frame
    // void upload_sprite_data() {
    //     m_sprite_ubo.upload(SpriteUniformData{
    //         glsl::Vec4{
    //             m_slice_offset.x,
    //             m_slice_offset.y,
    //             m_slice_size.x,
    //             m_slice_size.y
    //         },
    //         m_overlap_color
    //     });
    // }

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

    // Sprite material properties
    //     std::shared_ptr<Texture> m_texture_ptr;
    //     vk::DescriptorSet m_sprite_dset;
    //     vk::Image m_image;
    //     Vec2 m_slice_offset;
    //     Vec2 m_slice_size;
    //     Vec4 m_overlap_color = {0.68, 0.68, 0.68, 0.f};

    // public:
    //     vk::UniformBuffer m_sprite_ubo;
};

} // namespace kzn