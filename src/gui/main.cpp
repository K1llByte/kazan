#include "kazan.hpp"

int main(void)
{
    using namespace kzn;
    auto window = Window("Kazui", 1000, 800);
    auto renderer = Renderer(&window);
    auto model_pass = ModelPass(&renderer);

    // Models
    // auto monki = Model::load("assets/models/monkey.obj");
    // auto model_pipeline = ModelPipeline();

    // pipeline_manager["mesh_phong"]
    // pipeline_manager["mesh_wireframe"]

    while(!window.is_closed())
    {
        window.poll_events();

        // renderer.render_frame([&](auto& cmd_buffer) {
        //     // Begin render pass
        //     model_pass.render(cmd_buffer, [&] {
        //         model_pipeline.bind();

        //         monki.draw(cmd_buffer);
        //     });
        // });
    }
}