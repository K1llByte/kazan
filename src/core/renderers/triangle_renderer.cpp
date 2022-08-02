#include "core/renderers/triangle_renderer.hpp"

#include "core/camera.hpp"
#include "core/model.hpp"

namespace kzn
{
    TriangleRenderer::TriangleRenderer(Renderer* _renderer)
        : renderer(_renderer),
        render_pass(kzn::vk::simple_depth_render_pass(
            Context::device(),
            Context::swapchain().get_surface_format().format)),
        pipeline(vk::Pipeline(
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
        render_pass.create_framebuffers(Context::swapchain());
        renderer->add_render_pass(render_pass);
    }

    void TriangleRenderer::bind(vk::CommandBuffer& cmd_buffer)
    {
        render_pass.begin(cmd_buffer, Context::swapchain());

        auto& viewport = renderer->current_viewport();
        auto& scissor = renderer->current_scissor();
        
        pipeline.set_viewport(cmd_buffer, viewport);
        pipeline.set_scissor(cmd_buffer, scissor);
        pipeline.bind(cmd_buffer);
    }

    void TriangleRenderer::unbind(vk::CommandBuffer& cmd_buffer)
    {
        render_pass.end(cmd_buffer);
    }

    void TriangleRenderer::draw(vk::CommandBuffer& cmd_buffer)
    {
        vkCmdDraw(cmd_buffer.vk_command_buffer(), 3, 1, 0, 0);
    }
} // namespace kzn