#include "kazan.hpp"

int main(void)
{
    using namespace kzn;
    auto window = Window("Kazui", 1000, 800);
    auto renderer = Renderer(&window);
    auto triangle_pass = TrianglePass(&renderer);

    while(!window.is_closed())
    {
        window.poll_events();

        renderer.render_frame([&](auto& cmd_buffer)
        {
            triangle_pass.render(cmd_buffer, [&cmd_buffer] {
                vk::cmd_draw(cmd_buffer, 3, 1, 0, 0);
            });
        });
        
        // renderer.render_frame([&](auto& ctx) {
        //     // Begin render pass with color and depth buffer
        //     geometry_pass.render(framebuffer, [&] { // Begins render pass
        //         // Draw and other stuff ...
        //         transform_ubo.bind(ctx.cmd_buffer);
        //         model_pipeline.bind(ctx.cmd_buffer);
        //         example_model.draw(ctx.cmd_buffer);
        //     }); // Ends render pass
        // });
    }
}