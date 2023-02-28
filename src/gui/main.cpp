#include "kazan.hpp"

#include "gui/default_pipelines.hpp"

int main(void)
{
    using namespace kzn;
    auto window = Window("Kazui", 1000, 800);
    auto renderer = Renderer(&window);

    auto triangle_pass = TrianglePass(&renderer);
    auto triangle_pipeline = create_triangle_pipeline(
        triangle_pass.get_render_pass()
    );

    // NOTE: Maybe include allocator in vk::Device
    // auto desc_set = desc_set_allocator.allocate(triangle_pipeline.descriptor_set_layouts[0]);

    // desc_set.update({
    //     ubo.info(),
    //     texture.info(),
    // });
    
    // Models
    // auto monki = Model::load("assets/models/monkey.obj");
    // auto model_pipeline = ModelPipeline();

    // pipeline_manager["phong"]
    // pipeline_manager["phong_wireframe"]



    while(!window.is_closed())
    {
        window.poll_events();

        renderer.render_frame([&](auto& cmd_buffer) {
            // Begin render pass
            triangle_pass.render(cmd_buffer, [&] {
                triangle_pipeline.bind(cmd_buffer);
                vk::cmd_set_viewport(cmd_buffer, renderer.current_viewport());
                vk::cmd_set_scissor(cmd_buffer, renderer.current_scissor());
                vk::cmd_draw(cmd_buffer, 3, 1, 0, 0);

                // phong_pipeline.bind(cmd_buffer);
                // desc_set.bind(cmd_buffer, phong_pipeline.layout());
                // vk::cmd_set_viewport(cmd_buffer, renderer.current_viewport());
                // vk::cmd_set_scissor(cmd_buffer, renderer.current_scissor());
                // phong_pipeline.bind(cmd_buffer);
            });
        });
    }
}