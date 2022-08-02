#include "core/renderers/model_renderer.hpp"

#include "core/model.hpp"

namespace kzn
{
    ModelRenderer::ModelRenderer(Renderer* _renderer)
        : renderer(_renderer),
        render_pass(kzn::vk::simple_depth_render_pass(
            Context::device(),
            Context::swapchain().get_surface_format().format)),
        allocator(&Context::device()),
        cache(&Context::device()),
        ubo(&Context::device(), sizeof(Tmp)),
        desc_set(&Context::device(), allocator, cache, {
            vk::BufferBinding::uniform(0, ubo.info())
        }),
        pipeline(vk::Pipeline(
            &Context::device(),
            "assets/shaders/mesh/mesh.vert.spv",
            "assets/shaders/mesh/mesh.frag.spv",
            vk::PipelineConfigBuilder(
                    vk::PipelineLayoutBuilder(&Context::device())
                        .add_push_constant(sizeof(PVM), VK_SHADER_STAGE_ALL_GRAPHICS) 
                        .add_descriptor_set_layout(desc_set.layout())
                        .build(),
                    render_pass
                )
                .set_polygon_mode(VK_POLYGON_MODE_FILL)
                .set_dynamic_states({VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR})
                // .set_vtx_input<glm::vec3, glm::vec3, glm::vec3, glm::vec2>()
                .set_type_vtx_input<Vertex>()
                .build()
            )
        ),
        wireframe_pipeline(vk::Pipeline(
            &Context::device(),
            "assets/shaders/mesh/mesh_no_light.vert.spv",
            "assets/shaders/mesh/mesh_no_light.frag.spv",
            vk::PipelineConfigBuilder(
                    vk::PipelineLayoutBuilder(&Context::device())
                        .add_push_constant(sizeof(PVM), VK_SHADER_STAGE_ALL_GRAPHICS)
                        .add_descriptor_set_layout(desc_set.layout())
                        .build(),
                    render_pass
                )
                .set_polygon_mode(VK_POLYGON_MODE_LINE)
                .set_dynamic_states({VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR})
                .set_type_vtx_input<Vertex>()
                .build()
            )
        )
    {
        render_pass.create_framebuffers(Context::swapchain());
        renderer->add_render_pass(render_pass);
    }

    void ModelRenderer::bind(vk::CommandBuffer& cmd_buffer, bool render_wireframe)
    {
        // auto& cmd_buffer = renderer->current_cmd_buffer();
        render_pass.begin(cmd_buffer, Context::swapchain());

        // auto& cmd_buffer = renderer->current_cmd_buffer();
        auto& viewport = renderer->current_viewport();
        auto& scissor = renderer->current_scissor();
        
        vk::Pipeline* target_pipeline = &pipeline;
        if(render_wireframe)
        {
            target_pipeline = &wireframe_pipeline;
        }
        target_pipeline->set_viewport(cmd_buffer, viewport);
        target_pipeline->set_scissor(cmd_buffer, scissor);
        target_pipeline->bind(cmd_buffer);

        // Bind descriptor set
        desc_set.bind(cmd_buffer, target_pipeline->layout());
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