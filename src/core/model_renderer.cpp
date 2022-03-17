#include "core/model_renderer.hpp"

namespace kzn
{
    ModelRenderer::ModelRenderer(Renderer* _renderer)
        : renderer(_renderer),
        render_pass(vk::RenderPassBuilder(&renderer->device)
            .set_format(renderer->swapchain.get_surface_format().format)
            .build()),
        pipeline(vk::Pipeline(
            &renderer->device,
            "assets/shaders/triangle/triangle.vert.spv",
            "assets/shaders/triangle/triangle.frag.spv",
            vk::PipelineConfigBuilder(vk::PipelineLayoutBuilder(&renderer->device).build(), render_pass)
                .set_dynamic_states({VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR})
                // .set_vtx_input<glm::vec2, glm::vec3>()
                .build()))
    {
        render_pass.create_framebuffers(renderer->swapchain);
        renderer->add_render_pass(render_pass);
    }

    ModelRenderer::~ModelRenderer()
    {
        
    }

    void ModelRenderer::draw()
    {
        auto& cmd_buffer = renderer->current_cmd_buffer();
        auto& viewport = renderer->current_viewport();
        auto& scissor = renderer->current_scissor();
        render_pass.begin(cmd_buffer, renderer->swapchain);
        pipeline.set_viewport(cmd_buffer, viewport);
        pipeline.set_scissor(cmd_buffer, scissor);
        pipeline.bind(cmd_buffer);

        // Model draw commands
        // vbo.bind(cmd_buffer);
        // ibo.bind(cmd_buffer);
        // vkCmdDrawIndexed(cmd_buffer.vk_command_buffer(), vertices.size(), 1, 0, 0, 0);
        vkCmdDraw(cmd_buffer.vk_command_buffer(), 3, 1, 0, 0);
    }
} // namespace kzn