#include "kazan.hpp"

using namespace kzn;

//////////////////////// Example 4 ////////////////////////
// This example uses the kazan higher level API to generate
// and render a sphere with a hardcoded directional light
// in the shader. An alternative pipeline is bound when the
// space key is pressed where only the wireframe is
// rendered.
///////////////////////////////////////////////////////////

int main() try
{
    auto window = Window("Kazan", 1700, 800);
    auto renderer = Renderer(&window);

    auto model_renderer = ModelRenderer(&renderer);

    auto model = kzn::sphere(1.f, 20, 20);

    Camera camera;
    camera.lookat_target(glm::vec3(5.f, 2.f, -2.f), glm::vec3(0.f, 0.f, 0.f));

    float counter = 0;
    bool render_wireframe_begin_state = false;
    bool render_wireframe = false;
    while(!window.should_close())
    {
        // Poll events
        window.poll_events();

        // Update
        camera.set_perspective(glm::radians(50.f), window.aspect_ratio(), 0.1f, 100.f);

        // Model rotation
        model.transform.rotation = glm::vec3{0.f, glm::radians(counter), 0.f};

        switch (glfwGetKey(window.glfw_ptr(), GLFW_KEY_SPACE))
        {
            case GLFW_PRESS:
                render_wireframe_begin_state = true;
                break;
            case GLFW_RELEASE:
                if(render_wireframe_begin_state)
                    render_wireframe = !render_wireframe;
                render_wireframe_begin_state = false;
                break;
        }

        // Begin and End frame
        renderer.render_frame([&](auto& cmd_buffer)
        {
            PVM pvm {
                camera.projection() * camera.view(),
                model.transform.mat4()
            };

            model_renderer.bind(cmd_buffer, render_wireframe);
                // Draw model
                pvm.model = model.transform.mat4();
                model_renderer.push(cmd_buffer, pvm);
                model.draw(cmd_buffer);
            model_renderer.unbind(cmd_buffer);
        });

        ++counter;

        // Show FPS
        window.set_title(fmt::format("FPS: {:.0f}", (1. / Time::delta())));
    }
    renderer.wait_idle();
}
catch(const vk::ResultError& re)
{
    Log::error("Code: {}", re.raw());
}