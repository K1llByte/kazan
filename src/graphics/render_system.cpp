#include "render_system.hpp"

#include "core/assert.hpp"
#include "core/console.hpp"
#include "core/log.hpp"
#include "core/type.hpp"
#include "ecs/entity.hpp"

#include "events/events.hpp"
#include "graphics/renderer.hpp"
#include "graphics/stages/imgui_stage.hpp"
#include "graphics/utils.hpp"
#include "math/transform.hpp"
#include "math/types.hpp"
#include "vk/buffer.hpp"
#include "vk/dset_layout.hpp"
#include "vk/image.hpp"
#include "vk/uniform.hpp"

// clang-format off
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
// clang-format on

#include <optional>
#include <vulkan/vulkan_core.h>

#include <array>

namespace kzn {

std::vector<vk::Framebuffer> create_framebuffers(
    vk::RenderPass& render_pass,
    std::optional<Ref<vk::Image>> depth_opt = std::nullopt
) {
    // Get swapchain color attachment images
    const size_t num_swapchain_images =
        Renderer::swapchain().image_views().size();
    const auto swapchain_extent = Renderer::swapchain().extent();

    std::vector<vk::Framebuffer> framebuffers;
    framebuffers.reserve(num_swapchain_images);

    // Create framebuffers with depth attachment
    if (depth_opt.has_value()) {
        for (auto image_view : Renderer::swapchain().image_views()) {
            framebuffers.emplace_back(
                render_pass,
                std::array{image_view, depth_opt->get().vk_image_view()},
                swapchain_extent
            );
        }
    }
    // Create framebuffers without depth attachment
    else {
        for (auto image_view : Renderer::swapchain().image_views()) {
            framebuffers.emplace_back(
                render_pass, std::array{image_view}, swapchain_extent
            );
        }
    }

    return framebuffers;
}

RenderSystem::RenderSystem()
    : m_screen_render_pass(simple_depth_pass(
          Renderer::device(),
          Renderer::swapchain().image_format()
      ))
    , m_depth_image(
          Renderer::device(),
          {Renderer::swapchain().extent().width,
           Renderer::swapchain().extent().height,
           1},
          VK_FORMAT_D32_SFLOAT,
          VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
          VK_IMAGE_ASPECT_DEPTH_BIT
      )
    , m_framebuffers{create_framebuffers(m_screen_render_pass, m_depth_image)}
    , m_camera_ubo{Renderer::device(), sizeof(CameraUniformData)}
    , m_camera_dset{Renderer::dset_allocator().allocate(
          Renderer::dset_layout_cache().create_layout({
              vk::uniform_binding(0),
          })
      )}
    , m_sprite_stage(m_screen_render_pass, m_camera_dset)
    , m_debug_stage(m_screen_render_pass, m_camera_dset)
    , m_imgui_stage_opt(std::nullopt) {

    context<Console>().create_cmd("debug_render", [this]() {
        m_debug_stage.enable(!m_debug_stage.is_enabled());
    });

    // Update camera descriptor set to use camera ubo
    m_camera_dset.update({m_camera_ubo.info()});

    // Listen to renderer swapchain resize event
    listen(&RenderSystem::on_swapchain_resize);
    listen(&RenderSystem::on_editor_init);
}

RenderSystem::~RenderSystem() {
    context<Console>().delete_cmd("debug_render");

    Renderer::device().wait_idle();
}

void RenderSystem::update(float delta_time) {
    // Pre-render
    m_sprite_stage.pre_render();
    m_debug_stage.pre_render();
    if (m_imgui_stage_opt) {
        m_imgui_stage_opt->pre_render();
    }

    // Select rendering camera otherwise choose default camera params.
    entt::entity camera_entity = entt::null;
    Camera2DComponent* camera_ptr = nullptr;
    for (auto [entity, camera] :
         Registry::registry().view<Camera2DComponent>().each()) {
        camera_ptr = &camera;
        camera_entity = entity;
        break;
    }

    if (camera_ptr == nullptr) {
        Log::warning("No camera selected for rendering, using default "
                     "camera parameters");
        // Default camera shader data
        m_camera_ubo.upload(CameraUniformData{Vec2{0, 0}, 1, 1, 0});
    }
    else {
        if (camera_ptr->use_viewport_aspect_ratio) {
            auto extent = Renderer::swapchain().extent();
            camera_ptr->aspect_ratio =
                float(extent.width) / float(extent.height);
        }

        auto camera_data = CameraUniformData{
            .position = Vec2{0, 0},
            .zoom = camera_ptr->zoom,
            .aspect_ratio = camera_ptr->aspect_ratio,
            .rotation = 0.f,
        };
        auto transform_ptr =
            Registry::registry().try_get<Transform2DComponent>(camera_entity);
        if (transform_ptr != nullptr) {
            camera_data.rotation = transform_ptr->rotation;
            camera_data.position = transform_ptr->position;
        }

        m_camera_ubo.upload(camera_data);
    }

    // Start rendering frame
    Renderer::singleton().render_frame([&](auto& cmd_buffer) {
        const VkClearValue clear_color{
            .color = {m_clear_color.r, m_clear_color.y, m_clear_color.z, 1.0f},
        };
        const VkClearValue clear_depth{
            .depthStencil = {1.f, 0},
        };

        m_screen_render_pass.begin(
            cmd_buffer,
            m_framebuffers[Renderer::swapchain().current_index()],
            {clear_color, clear_depth}
        );

        m_sprite_stage.render(cmd_buffer);
        m_debug_stage.render(cmd_buffer);
        if (m_imgui_stage_opt) {
            m_imgui_stage_opt->render(cmd_buffer);
        }

        m_screen_render_pass.end(cmd_buffer);
    });
}

void RenderSystem::on_swapchain_resize(const SwapchainResizeEvent&) {
    m_depth_image = vk::Image(
        Renderer::device(),
        {Renderer::swapchain().extent().width,
         Renderer::swapchain().extent().height,
         1},
        VK_FORMAT_D32_SFLOAT,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );
    m_framebuffers = create_framebuffers(m_screen_render_pass, m_depth_image);
}

void RenderSystem::on_editor_init(const EditorInitEvent&) {
    KZN_ASSERT_MSG(
        !m_imgui_stage_opt.has_value(),
        "ImGuiStage already injected in RenderSystem"
    );

    m_imgui_stage_opt.emplace(m_screen_render_pass);
}

} // namespace kzn