#include "kazan.hpp"

using namespace kzn;

//////////////////////// Example 3 ////////////////////////
// This example uses the kazan higher level API to render 
// the same triangle as the previous example 2, with data 
// feeded by the application.
///////////////////////////////////////////////////////////

int main()
{
    auto window = Window("Kazan", 1700, 800);
    auto renderer = Renderer(&window);

    auto model_renderer = ModelRenderer(&renderer);
    while(!window.should_close())
    {
        // Poll events
        window.poll_events();

        // Begin and End frame
        renderer.render_frame([&]
        {
            model_renderer.bind();
                model_renderer.draw();
            model_renderer.unbind();
        });

        // Show FPS
        window.set_title(fmt::format("FPS: {:.0f}", (1. / Time::delta())));
    }
    renderer.wait_idle();
}