#include "core/model_renderer.hpp"

#include "core/camera.hpp"

namespace kzn
{
    ModelRenderer::ModelRenderer(Renderer* _renderer)
        : renderer(_renderer),
        render_pass(vk::RenderPassBuilder(&Context::device())
            .set_format(Context::swapchain().get_surface_format().format)
            .build()),
        pipeline(vk::Pipeline(
            &Context::device(),
            "assets/shaders/mesh/mesh.vert.spv",
            "assets/shaders/mesh/mesh.frag.spv",
            vk::PipelineConfigBuilder(
                    vk::PipelineLayoutBuilder(&Context::device())
                        .add_push_constant(sizeof(PVM), VK_SHADER_STAGE_ALL_GRAPHICS) 
                        .build(),
                    render_pass)
                .set_dynamic_states({VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR})
                .set_vtx_input<glm::vec3, glm::vec3, glm::vec3, glm::vec2>()
                .build()))
    {
        render_pass.create_framebuffers(Context::swapchain());
        renderer->add_render_pass(render_pass);
    }

    void ModelRenderer::bind(vk::CommandBuffer& cmd_buffer)
    {
        // auto& cmd_buffer = renderer->current_cmd_buffer();
        render_pass.begin(cmd_buffer, Context::swapchain());

        // auto& cmd_buffer = renderer->current_cmd_buffer();
        auto& viewport = renderer->current_viewport();
        auto& scissor = renderer->current_scissor();
        pipeline.set_viewport(cmd_buffer, viewport);
        pipeline.set_scissor(cmd_buffer, scissor);
        pipeline.bind(cmd_buffer);
    }

    void ModelRenderer::unbind(vk::CommandBuffer& cmd_buffer)
    {
        // auto& cmd_buffer = renderer->current_cmd_buffer();
        render_pass.end(cmd_buffer);
    }

    void ModelRenderer::draw(vk::CommandBuffer& cmd_buffer)
    {
        // Model draw commands
        // vkCmdDrawIndexed(cmd_buffer.vk_command_buffer(), vertices.size(), 1, 0, 0, 0);
        vkCmdDraw(cmd_buffer.vk_command_buffer(), 3, 1, 0, 0);
    }
} // namespace kzn