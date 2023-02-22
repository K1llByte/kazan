#include "core/renderers/triangle_pass.hpp"

#include "core/camera.hpp"
#include "core/model.hpp"
#include "vk/commands.hpp"

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
                .set_polygon_mode(VK_POLYGON_MODE_FILL)
                .set_dynamic_states({VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR})
                .set_front_face(VK_FRONT_FACE_CLOCKWISE)
                .build()
            )
        )
    {

        renderer->add_render_pass(render_pass);
    }

    void TrianglePass::render(vk::CommandBuffer& cmd_buffer, std::function<void(void)> func)
    {
        //////////////////////
        // Begin RenderPass //
        //////////////////////

        render_pass.begin(
            cmd_buffer,
            framebuffers.get(Context::swapchain().current_index())
        );

        auto& viewport = renderer->current_viewport();
        auto& scissor = renderer->current_scissor();
        
        vk::cmd_set_viewport(cmd_buffer, viewport);
        vk::cmd_set_scissor(cmd_buffer, scissor);
        pipeline.bind(cmd_buffer);

        ///////////////////
        // Register Cmds //
        ///////////////////

        func();

        ////////////////////
        // End RenderPass //
        ////////////////////

        render_pass.end(cmd_buffer);
    }

    void TrianglePass::draw(vk::CommandBuffer& cmd_buffer)
    {
        vkCmdDraw(cmd_buffer.vk_command_buffer(), 3, 1, 0, 0);
    }

    void TrianglePass::on_resize(const ResizeEvent& e) {
        framebuffers.recreate(
            renderer->get_render_images(),
            renderer->get_render_extent()
        );
    }
} // namespace kzn