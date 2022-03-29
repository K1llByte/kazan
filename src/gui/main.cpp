#include "kazan.hpp"

// Kazui headers
#include "gui/camera_controller.hpp"

using namespace kzn;

int main() try
{
    auto window = Window("Kazan", 1700, 800);
    auto input = window.input_handler();
    auto renderer = Renderer(&window);

    auto model_renderer = ModelRenderer(&renderer);

    // auto model = Model::load("assets/models/monkey.obj");
    auto model = kzn::icosahedron(0.8f, 30, true);
    model.transform.position += glm::vec3{0.f, 0.f, 1.f};
    // auto model2 = kzn::sphere(0.8f, 17, 16);
    auto model2 = kzn::sphere(0.8f, 50, 50);
    model2.transform.position += glm::vec3{0.f, 0.f, -1.f};

    Camera camera;
    camera.lookat_target(glm::vec3{5.f, 2.f, 0.f}, glm::vec3{0.f, 0.f, 0.f});
    CameraController camera_controller(&window, &camera);



    float counter = 0;
    while(!window.should_close())
    {
        auto delta_time = Time::delta();
        // Poll events
        window.poll_events();

        // Update
        camera.set_perspective(glm::radians(50.f), window.aspect_ratio(), 0.1f, 100.f);
        camera_controller.update(delta_time);

        // Model rotation
        // model.transform.rotation = glm::vec3{0.f, glm::radians(counter), 0.f};

        static bool render_wireframe_begin_state = false;
        static bool render_wireframe = false;
        switch (input.get_key(GLFW_KEY_SPACE))
        {
            case GLFW_PRESS:
                if(!render_wireframe_begin_state)
                    render_wireframe = !render_wireframe;
                render_wireframe_begin_state = true;
                break;
            case GLFW_RELEASE:
                render_wireframe_begin_state = false;
                break;
        }

        constexpr float MOVE_SPEED = 0.01;
        if(input.is_key_pressed(GLFW_KEY_L))
            model.transform.position += glm::vec3{ 0.f, 0.f, MOVE_SPEED };
        if(input.is_key_pressed(GLFW_KEY_J))
            model.transform.position += glm::vec3{ 0.f, 0.f, -MOVE_SPEED };
        if(input.is_key_pressed(GLFW_KEY_I))
            model.transform.position += glm::vec3{ -MOVE_SPEED, 0.f, 0.f };
        if(input.is_key_pressed(GLFW_KEY_K))
            model.transform.position += glm::vec3{ MOVE_SPEED, 0.f, 0.f };
        
        // Begin and End frame
        renderer.render_frame([&](auto& cmd_buffer)
        {
            PVM pvm {
                camera.projection() * camera.view(),
                model.transform.mat4()
            };

            model_renderer.bind(cmd_buffer, render_wireframe);
                // Draw model 1
                pvm.model = model.transform.mat4();
                model_renderer.push(cmd_buffer, pvm);
                model.draw(cmd_buffer);

                // Draw model 2
                pvm.model = model2.transform.mat4();
                model_renderer.push(cmd_buffer, pvm);
                model2.draw(cmd_buffer);

                // // Draw model 3
                // pvm.model = model3.transform.mat4();
                // model_renderer.push(cmd_buffer, pvm);
                // model3.draw(cmd_buffer);
            model_renderer.unbind(cmd_buffer);
        });

        // Show FPS each second
        // auto delta_time = Time::delta();
        static double until_second = 0.;
        until_second += delta_time;
        if(until_second > 1.)
        {
            window.set_title(fmt::format("FPS: {:.0f}", (1. / delta_time)));
            until_second = 0.;
        }
        ++counter;
    }
    renderer.wait_idle();
}
catch(const vk::ResultError& re)
{
    Log::error("Code: {}", re.raw());
}