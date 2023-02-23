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
            // Begin render pass
            triangle_pass.render(cmd_buffer, [&] {
                // triangle_pipeline.bind();
                vk::cmd_draw(cmd_buffer, 4, 1, 0, 0);
            });
        });
    }
}