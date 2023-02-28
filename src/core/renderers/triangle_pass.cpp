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
    {

    }

    void TrianglePass::on_resize(const ResizeEvent&) {
        framebuffers.recreate(
            renderer->get_render_images(),
            renderer->get_render_extent()
        );
    }
} // namespace kzn