#include "render_system.hpp"

#include "core/assert.hpp"
#include "core/console.hpp"
#include "core/log.hpp"
#include "core/type.hpp"
#include "ecs/entity.hpp"
#include "ecs/scene.hpp"
#include "events/events.hpp"
#include "graphics/renderer.hpp"
#include "graphics/stages/imgui_stage.hpp"
#include "graphics/utils.hpp"
#include "math/transform.hpp"
#include "math/types.hpp"
#include "vk/buffer.hpp"
#include "vk/dset_layout.hpp"
#include "vk/image.hpp"
#include "graphics/camera.hpp"

// clang-format off
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
// clang-format on
#include <vulkan/vulkan_core.h>

#include <optional>
#include <array>

namespace kzn {

std::vector<vk::Framebuffer> create_framebuffers(
    Renderer& renderer,
    vk::RenderPass& render_pass,
    std::optional<Ref<vk::Image>> depth_opt = std::nullopt
) {
    // Get swapchain color attachment images
    const size_t num_swapchain_images =
        renderer.swapchain().image_views().size();
    const auto swapchain_extent = renderer.swapchain().extent();

    std::vector<vk::Framebuffer> framebuffers;
    framebuffers.reserve(num_swapchain_images);

    // Create framebuffers with depth attachment
    if (depth_opt.has_value()) {
        for (auto image_view : renderer.swapchain().image_views()) {
            framebuffers.emplace_back(
                render_pass,
                std::array{image_view, depth_opt->get().vk_image_view()},
                swapchain_extent
            );
        }
    }
    // Create framebuffers without depth attachment
    else {
        for (auto image_view : renderer.swapchain().image_views()) {
            framebuffers.emplace_back(
                render_pass, std::array{image_view}, swapchain_extent
            );
        }
    }

    return framebuffers;
}

RenderSystem::RenderSystem()
    : m_screen_render_pass(simple_depth_pass(
          context<Renderer>().device(),
          context<Renderer>().swapchain().image_format()
      ))
    , m_depth_image(
          context<Renderer>().device(),
          {context<Renderer>().swapchain().extent().width,
           context<Renderer>().swapchain().extent().height,
           1},
          VK_FORMAT_D32_SFLOAT,
          VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
          VK_IMAGE_ASPECT_DEPTH_BIT
      )
    , m_framebuffers{create_framebuffers(context<Renderer>(), m_screen_render_pass, m_depth_image)}
    , m_camera2d_ubo{context<Renderer>().device(), sizeof(Camera2DUniformData)}
    , m_camera2d_dset{context<Renderer>().device().dset_allocator().allocate(
          context<Renderer>().device().dset_layout_cache().layout({
              vk::uniform_binding(0),
          })
      )}
    , m_camera3d_ubo{context<Renderer>().device(), sizeof(Camera3DUniformData)}
    , m_camera3d_dset{context<Renderer>().device().dset_allocator().allocate(
          context<Renderer>().device().dset_layout_cache().layout({
              vk::uniform_binding(0),
          })
      )}
{
    // context<Console>().create_cmd("debug_render", [this]() {
    //     m_debug_stage.enable(!m_debug_stage.is_enabled());
    // });

    // Update camera descriptor set to use camera ubo
    m_camera2d_dset.update({m_camera2d_ubo.info()});
    // Update camera descriptor set to use camera ubo
    m_camera3d_dset.update({m_camera3d_ubo.info()});

    // Listen to renderer swapchain resize event
    listen(&RenderSystem::on_swapchain_resize);
    listen(&RenderSystem::on_editor_init);
}

RenderSystem::~RenderSystem() {
    // context<Console>().delete_cmd("debug_render");

    context<Renderer>().device().wait_idle();
}

void RenderSystem::update(Scene& scene, float delta_time) {
    auto& renderer = context<Renderer>();
    // Pre-render
    for(auto& render_stage_ptr : m_render_stages) {
        render_stage_ptr->pre_render(scene);
    }

    select_camera(scene);

    // Start rendering frame
    renderer.render_frame([&](auto& cmd_buffer) {
        const VkClearValue clear_color{
            .color = {m_clear_color.r, m_clear_color.y, m_clear_color.z, 1.0f},
        };
        const VkClearValue clear_depth{
            .depthStencil = {1.f, 0},
        };

        m_screen_render_pass.begin(
            cmd_buffer,
            m_framebuffers[renderer.swapchain().current_index()],
            {clear_color, clear_depth}
        );

        for(auto& render_stage_ptr : m_render_stages) {
            render_stage_ptr->render(scene, cmd_buffer);
        }

        m_screen_render_pass.end(cmd_buffer);
    });

    // Post-render
    for(auto& render_stage_ptr : m_render_stages) {
        render_stage_ptr->post_render(scene);
    }
}

void RenderSystem::select_camera(Scene& scene) {
    auto& renderer = context<Renderer>();
    
    ///////////////////////////////////////////////////////////////////////////
    // Camera 2D
    ///////////////////////////////////////////////////////////////////////////

    // Select rendering camera otherwise choose default camera params.
    EntityId camera2d_entity = entt::null;
    Camera2DComponent* camera2d_ptr = nullptr;
    for (auto [entity, camera] : scene.registry.registry().view<Camera2DComponent>().each()) {
        camera2d_ptr = &camera;
        camera2d_entity = entity;
        break;
    }

    if (camera2d_ptr == nullptr) {
        // Default camera shader data
        m_camera2d_ubo.upload(Camera2DUniformData{
            .position = Vec2{0, 0},
            .zoom = 1,
            .aspect_ratio = 1,
            .rotation = 0
        });
    }
    else {
        if (camera2d_ptr->use_viewport_aspect_ratio) {
            auto extent = renderer.swapchain().extent();
            camera2d_ptr->aspect_ratio =
                float(extent.width) / float(extent.height);
        }

        auto camera_data = Camera2DUniformData{
            .position = Vec2{0},
            .zoom = camera2d_ptr->zoom,
            .aspect_ratio = camera2d_ptr->aspect_ratio,
            .rotation = 0.f,
        };
        auto transform_ptr =
            scene.registry.registry().try_get<Transform2DComponent>(camera2d_entity);
        if (transform_ptr != nullptr) {
            camera_data.rotation = transform_ptr->rotation;
            camera_data.position = transform_ptr->position;
        }

        m_camera2d_ubo.upload(camera_data);
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // Camera 3D
    ///////////////////////////////////////////////////////////////////////////

    // Select rendering camera otherwise choose default camera params.
    EntityId camera3d_entity = entt::null;
    Camera3DComponent* camera3d_ptr = nullptr;
    for (auto [entity, camera] : scene.registry.registry().view<Camera3DComponent>().each()) {
        camera3d_ptr = &camera;
        camera3d_entity = entity;
        break;
    }

    if (camera3d_ptr == nullptr) {
        // Default camera shader data
        m_camera3d_ubo.upload(Camera3DUniformData{
            .aspect_ratio = 1,
            .fov_v = glm::radians(100.f),
            .position = Vec3{0},
            .forward = Vec3{0,0,-1},
            .up = Vec3{0,-1,0},
            .proj_view = Mat4{},
        });
    }
    else {
        if (camera3d_ptr->use_viewport_aspect_ratio) {
            auto extent = renderer.swapchain().extent();
            camera3d_ptr->aspect_ratio =
                float(extent.width) / float(extent.height);
        }

        const float fov_v = glm::radians(camera3d_ptr->fov_v);
        const Mat4 proj_mat = camera3d_ptr->projection_matrix();
        const Mat4 view_mat = camera3d_ptr->view_matrix();
        auto camera_data = Camera3DUniformData{
            .aspect_ratio = camera3d_ptr->aspect_ratio,
            .fov_v = fov_v,
            .position = camera3d_ptr->position,
            .forward = camera3d_ptr->forward,
            .up = camera3d_ptr->up,
            .proj_view = proj_mat * view_mat,
        };
        m_camera3d_ubo.upload(camera_data);
    }

    if(camera2d_ptr == nullptr && camera3d_ptr == nullptr) {
        Log::warning("No camera selected for rendering, using default camera parameters");
    }
}

void RenderSystem::on_swapchain_resize(const SwapchainResizeEvent&) {
    auto& renderer = context<Renderer>();
    const auto extent = renderer.swapchain().extent();
    m_depth_image = vk::Image(
        renderer.device(),
        {extent.width,extent.height, 1},
        VK_FORMAT_D32_SFLOAT,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );
    m_framebuffers = create_framebuffers(renderer, m_screen_render_pass, m_depth_image);
}

void RenderSystem::on_editor_init(const EditorInitEvent&) {
    static bool missing_imgui_stage = true;
    KZN_ASSERT_MSG(
        missing_imgui_stage,
        "ImGuiStage already injected in RenderSystem"
    );

    emplace_stage<ImGuiStage>(
        context<Renderer>(),
        m_screen_render_pass
    );
    missing_imgui_stage = false;

}

} // namespace kzn