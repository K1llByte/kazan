#pragma once

#include "core/assert.hpp"
#include "graphics/renderer.hpp"
#include "graphics/texture.hpp"
#include "math/types.hpp"
#include "resources/resources.hpp"
#include "vk/buffer.hpp"
#include "vk/cmd_buffer.hpp"
#include "vk/dset.hpp"
#include "vk/functions.hpp"
#include "vk/image.hpp"
#include "vk/pipeline.hpp"
#include "vk/uniform.hpp"
#include <filesystem>
#include <optional>

namespace kzn {

struct SpriteUniformData {
    glsl::Vec4 slice;
    glsl::Vec4 overlap_color;
};

struct SpriteMaterialRenderData {
    SpriteMaterialRenderData(
        Renderer& renderer,
        std::shared_ptr<Texture>& texture_ptr,
        Vec2 slice_offset,
        Vec2 slice_size,
        Vec4 overlap_color
    )
        : material_dset{renderer.dset_allocator().allocate(
              renderer.dset_layout_cache().create_layout({
                  vk::sampler_binding(0),
                  vk::uniform_binding(1),
              })
          )}
        , albedo_image(renderer.device(), texture_ptr->extent())
        , material_ubo{renderer.device(), sizeof(SpriteUniformData)} {
        // Upload texture data to gpu image memory
        albedo_image.upload(texture_ptr->data());
        // Update material dset and upload data
        material_dset.update({albedo_image.info(), material_ubo.info()});
        material_ubo.upload(SpriteUniformData{
            glsl::Vec4{
                slice_offset.x, slice_offset.y, slice_size.x, slice_size.y
            },
            overlap_color
        });
    }

    ~SpriteMaterialRenderData() {}

    vk::DescriptorSet material_dset;
    vk::Image albedo_image;
    vk::UniformBuffer material_ubo;
};

//! Sprite texture and parameters to use in shader
class SpriteMaterial {
public:
    // Ctor
    SpriteMaterial(const std::filesystem::path& albedo_path)
        : m_texture_ptr(Resources::find_or_load<Texture>(albedo_path)) {}
    // Dtor
    ~SpriteMaterial() = default;

    void set_slice(Vec2 offset, Vec2 size) {
        m_slice_offset = offset;
        m_slice_size = size;
        m_was_modified = true;
    }

    void set_overlap_color(Vec4 overlap_color) {
        m_overlap_color = overlap_color;
        m_was_modified = true;
    }

    [[nodiscard]]
    constexpr bool has_render_data() const {
        return m_render_data_opt.has_value();
    }

    void create_render_data(Renderer& renderer) {
        m_render_data_opt.emplace(
            renderer, m_texture_ptr, m_slice_offset, m_slice_size, m_overlap_color
        );
        m_was_modified = false;
    }

    void destroy_render_data() { m_render_data_opt = std::nullopt; }

    void update_render_data() {
        if (m_was_modified) {
            m_render_data_opt->material_ubo.upload(SpriteUniformData{
                glsl::Vec4{
                    m_slice_offset.x,
                    m_slice_offset.y,
                    m_slice_size.x,
                    m_slice_size.y
                },
                m_overlap_color
            });
            m_was_modified = false;
        }
    }

    std::optional<SpriteMaterialRenderData>& render_data() {
        return m_render_data_opt;
    }

private:
    std::shared_ptr<Texture> m_texture_ptr;
    Vec2 m_slice_offset = {0, 0};
    Vec2 m_slice_size = {1, 1};
    Vec4 m_overlap_color = {0, 0, 0, 0};
    bool m_was_modified = false;
    std::optional<SpriteMaterialRenderData> m_render_data_opt = std::nullopt;
};

} // namespace kzn