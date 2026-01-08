#pragma once

#include "ecs/system.hpp"
#include "events/event_manager.hpp"
#include "events/events.hpp"
#include "graphics/debug_render.hpp"
#include "graphics/renderer.hpp"
#include "graphics/sprite_geom_cache.hpp"
#include "graphics/stages/debug_stage.hpp"
#include "graphics/stages/imgui_stage.hpp"
#include "graphics/stages/sprite_stage.hpp"
#include "graphics/texture.hpp"
#include "math/transform.hpp"
#include "math/types.hpp"
#include "resources/resources.hpp"
#include "vk/buffer.hpp"
#include "vk/dset.hpp"
#include "vk/dset_layout.hpp"
#include "vk/image.hpp"
#include "vk/pipeline.hpp"
#include "vk/render_pass.hpp"
#include "vk/uniform.hpp"

#include "boost/pfr/core_name.hpp"

#include <array>
#include <cstdint>
#include <memory>
#include <span>
#include <string_view>
#include <vulkan/vulkan_core.h>

#define KZN_ENABLE_EDITOR

namespace kzn {

struct Camera2DComponent {
    float zoom = 1.f;
    float aspect_ratio = 1.f;
    bool use_viewport_aspect_ratio = true;

    [[nodiscard]]
    constexpr Mat4 matrix() const {
        // Initialize with identity
        auto proj_mat = Mat4{};
        // Aspect ratio multiplier
        proj_mat[0].x = 1.f / aspect_ratio;
        return proj_mat;
    }
};

//! ECS System for rendering
class RenderSystem
    : public System
    , public EventListener {
public:
    // Ctor
    RenderSystem();
    // Copy
    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;
    // Move
    RenderSystem(RenderSystem&&) = delete;
    RenderSystem& operator=(RenderSystem&&) = delete;
    // Dtor
    ~RenderSystem() override;

    void update(float delta_time) override;

private:
    struct CameraUniformData {
        glsl::Vec2 position;
        glsl::Float zoom;
        glsl::Float aspect_ratio;
        glsl::Float rotation;
    };

private:
    // Render system specific data
    Vec3 m_clear_color{0.01f, 0.01f, 0.01f};
    vk::RenderPass m_screen_render_pass;
    vk::Image m_depth_image;
    std::vector<vk::Framebuffer> m_framebuffers;

    // Camera data shared between stages
    vk::UniformBuffer m_camera_ubo;
    vk::DescriptorSet m_camera_dset;

    // Stages
    SpriteStage m_sprite_stage;
    DebugStage m_debug_stage;
    ImGuiStage m_imgui_stage;

private:
    void on_swapchain_resize(const SwapchainResizeEvent&);
};

} // namespace kzn