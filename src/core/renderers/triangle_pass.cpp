#include "core/renderers/triangle_pass.hpp"

#include "core/camera.hpp"
#include "core/model.hpp"

namespace kzn
{
    TrianglePass::TrianglePass(Renderer* _renderer)
        : EventHandlers{
            kzn::register_event_handler(this, &TrianglePass::on_resize)
        }
        , renderer(_renderer)
        , render_pass(kzn::vk::simple_depth_render_pass(
            Context::device(),
            Context::swapchain().get_surface_format().format))
        // Initialize framebuffers
        , framebuffers(
            &Context::device(),
            &render_pass,
            renderer->get_render_images(),
            renderer->get_render_extent())
        , pipeline(vk::Pipeline(
            &Context::device(),
            "assets/shaders/triangle/triangle.vert.spv",
            "assets/shaders/triangle/triangle.frag.spv",
            vk::PipelineConfigBuilder(
                    vk::PipelineLayoutBuilder(&Context::device())
                        .build(),
                    render_pass
                )
                .set_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN)
                .set_polygon_mode(VK_POLYGON_MODE_FILL)
                .set_dynamic_states({VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR})
                .set_front_face(VK_FRONT_FACE_CLOCKWISE)
                .build()
            )
        )
    {

    }

    void TrianglePass::on_resize(const ResizeEvent&) {
        framebuffers.recreate(
            renderer->get_render_images(),
            renderer->get_render_extent()
        );
    }
} // namespace kzn