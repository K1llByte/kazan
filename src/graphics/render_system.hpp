#pragma once

#include "ecs/scene.hpp"
#include "ecs/system.hpp"
#include "events/event_manager.hpp"
#include "events/events.hpp"
#include "graphics/stages/render_stage.hpp"
#include "vk/buffer.hpp"
#include "vk/dset.hpp"
#include "vk/image.hpp"
#include "vk/render_pass.hpp"
#include "vk/uniform.hpp"

#include <boost/pfr/core_name.hpp>
#include <vulkan/vulkan_core.h>

#include <utility>
#include <memory>

#define KZN_ENABLE_EDITOR

namespace kzn {

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

    [[nodiscard]]
    vk::RenderPass& screen_render_pass() {
        return m_screen_render_pass;
    }

    [[nodiscard]]
    vk::DescriptorSet& camera2d_dset() {
        return m_camera2d_dset;
    }

    [[nodiscard]]
    vk::DescriptorSet& camera3d_dset() {
        return m_camera3d_dset;
    }

    template<typename T, typename ...Args>
    void emplace_stage(Args&&... args) {
        m_render_stages.push_back(
            std::make_unique<T>(std::forward<Args>(args)...)
        );
    }

    void update(Scene& scene, float delta_time) override;

private:
    struct Camera2DUniformData {
        glsl::Vec2 position;
        glsl::Float zoom;
        glsl::Float aspect_ratio;
        glsl::Float rotation;
    };

    struct Camera3DUniformData {
        glsl::Float aspect_ratio;
        glsl::Float fov_v;
        glsl::Vec3 position;
        glsl::Vec3 forward;
        glsl::Vec3 up;
    };

private:
    void select_camera(Scene& scene);

private:
    // Render system specific data
    Vec3 m_clear_color{0.01f, 0.01f, 0.01f};
    vk::RenderPass m_screen_render_pass;
    vk::Image m_depth_image;
    std::vector<vk::Framebuffer> m_framebuffers;

    // Camera data shared between stages
    vk::UniformBuffer m_camera2d_ubo;
    vk::DescriptorSet m_camera2d_dset;
    vk::UniformBuffer m_camera3d_ubo;
    vk::DescriptorSet m_camera3d_dset;

    // Stages
    std::vector<std::unique_ptr<RenderStage>> m_render_stages;

private:
    void on_swapchain_resize(const SwapchainResizeEvent&);
    void on_editor_init(const EditorInitEvent&);
};

} // namespace kzn