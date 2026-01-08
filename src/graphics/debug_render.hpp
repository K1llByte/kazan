#pragma once

#include "graphics/renderer.hpp"
#include "math/types.hpp"
#include "vk/buffer.hpp"
#include <optional>
#include <vector>

namespace kzn {

struct LineDraw {
    Vec2 position;
    Vec3 color;
};

//! An immediate debug render helper class to draw basic geometry on the screen.
class DebugRender {
public:
    // Ctor
    DebugRender() {
        constexpr std::size_t default_size = 64;
        m_line_draw_list.reserve(default_size);
    }
    // Copy
    DebugRender(const DebugRender&) = delete;
    DebugRender& operator=(const DebugRender&) = delete;
    // Move
    DebugRender(DebugRender&&) = delete;
    DebugRender& operator=(DebugRender&&) = delete;
    // Dtor
    ~DebugRender() = default;

    void draw_rect(Vec2 position, Vec2 size, Vec3 color = Vec3{1.f, 1.f, 1.f}) {
        const Vec2 half_size = size / 2.f;

        m_line_draw_list.insert(
            m_line_draw_list.end(),
            {
                // Top
                LineDraw{position + half_size * Vec2{-1, 1}, color},
                LineDraw{position + half_size * Vec2{1, 1}, color},
                // Right
                LineDraw{position + half_size * Vec2{1, 1}, color},
                LineDraw{position + half_size * Vec2{1, -1}, color},
                // Bottom
                LineDraw{position + half_size * Vec2{1, -1}, color},
                LineDraw{position + half_size * Vec2{-1, -1}, color},
                // Left
                LineDraw{position + half_size * Vec2{-1, -1}, color},
                LineDraw{position + half_size * Vec2{-1, 1}, color},
            }
        );
    }

    void draw_line(
        Vec2 position1,
        Vec2 position2,
        Vec3 color = Vec3{1.f, 1.f, 1.f}
    ) {
        m_line_draw_list.insert(
            m_line_draw_list.end(),
            {LineDraw{position1, color}, LineDraw{position2, color}}
        );
    }

    void set_line_width(float width) { m_width = width; }

    [[nodiscard]]
    float line_width() const {
        return m_width;
    }

    void clear() { m_line_draw_list.clear(); }

    //! Auxiliary method to create the temporary immediate vbo to be used by the
    //! debug render stage.
    //! \return The vertex buffer to render lines or nullopt of there are
    //! not lines to be drawn.
    [[nodiscard]]
    std::optional<vk::VertexBuffer> create_debug_vbo() {
        if (m_line_draw_list.empty()) {
            return std::nullopt;
        }
        auto vbo = vk::VertexBuffer(
            Renderer::device(), sizeof(LineDraw) * m_line_draw_list.size()
        );
        vbo.upload(static_cast<const void*>(m_line_draw_list.data()));
        return vbo;
    }

    [[nodiscard]]
    const std::vector<LineDraw>& lines() const {
        return m_line_draw_list;
    }

private:
    std::vector<LineDraw> m_line_draw_list;
    float m_width = 1.0f;
};

} // namespace kzn