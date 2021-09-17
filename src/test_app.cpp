#include "test_app.hpp"

#include "simple_render_system.hpp"
#include "camera.hpp"
#include "camera_controller.hpp"

#include <chrono>

// Debug
#include <iostream>

namespace kzn
{

TestApp::TestApp()
    : _window("Hello Kazan", WIDTH, HEIGHT),
    _instance()
{
    _instance.enable_extensions(_window.required_extensions())
        .enable_validation_layers()
        .set_debug_messeger()
        .init();

    // Is it needed to preserve the surface handle?
    auto surface = _window.create_surface(_instance);

    // This instance will be discarded after
    // the scope of this ctor.
    // TODO: make a move ctor for Device to 
    // receive a PhysicalDevice.
    auto physical_device = PhysicalDeviceSelector(_instance, surface)
        .select();

    _device = Device(physical_device);
    _device.init();

    // _renderer = std::make_unique<Renderer>(_window, _device);
    _renderer = Renderer(&_window, &_device);

    // After engine initialization
    load_game_objects();
}


TestApp::~TestApp()
{
    _renderer.cleanup();
    _window.destroy_surface(_instance);
}


void TestApp::run()
{
    SimpleRenderSystem render_system{_device, _renderer.render_pass()};
    Camera camera{};
    camera.lookat_direction(glm::vec3(0.f), glm::vec3(0.0f, 0.f, 1.f));
    CameraController cam_controller(_window, camera);

    _renderer.delta_time();

    while(!_window.should_close())
    {
        glfwPollEvents();

        float aspect = _renderer.aspect_ratio();
        float dt = _renderer.delta_time();
        // std::cout << "fps: " << ( 1 / dt ) << '\n';
        // camera.set_orthographic(-aspect, aspect, -1, 1, -1, 1);
        camera.set_prespective(glm::radians(50.f), aspect, 0.1f, 100.0f);

        cam_controller.update(dt);

        if(auto command_buffer = _renderer.begin_frame())
        {
            _renderer.begin_render_pass(command_buffer);
            render_system.render_game_objects(command_buffer, _game_objects, camera);
            _renderer.end_render_pass(command_buffer);
            _renderer.end_frame();
        }
    }

    _device.wait_idle();
}


void TestApp::load_game_objects()
{
    // std::vector<Model::Vertex> vertices{
    //     Model::Vertex{
    //         .position{ 0.0f, -0.5f, 0.0f },
    //         .normal  { 0.0f,  0.0f, 0.0f },
    //         .color   { 1.0f,  0.0f, 0.0f },
    //     },
    //     Model::Vertex{
    //         .position{ 0.5f,  0.5f, 0.0f },
    //         .normal  { 0.0f,  0.0f, 0.0f },
    //         .color   { 1.0f,  1.0f, 0.0f },
    //     },
    //     Model::Vertex{
    //         .position{-0.5f,  0.5f, 0.0f },
    //         .normal  { 0.0f,  0.0f, 0.0f },
    //         .color   { 1.0f,  0.0f, 1.0f },
    //     }
    // };

    // std::vector<Model::Vertex> vertices{
    //     {{ 0.0f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    //     {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    //     {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    // };

    std::vector<Model::Vertex> floor{
        {{-1.f, 0.f, -1.f}, {0.0f, 0.0f, 0.0f}, {.2f, .2f, .2f}},
        {{ 1.f, 0.f,  1.f}, {0.0f, 0.0f, 0.0f}, {.2f, .2f, .2f}},
        {{-1.f, 0.f,  1.f}, {0.0f, 0.0f, 0.0f}, {.2f, .2f, .2f}},
        {{-1.f, 0.f, -1.f}, {0.0f, 0.0f, 0.0f}, {.2f, .2f, .2f}},
        {{ 1.f, 0.f, -1.f}, {0.0f, 0.0f, 0.0f}, {.2f, .2f, .2f}},
        {{ 1.f, 0.f,  1.f}, {0.0f, 0.0f, 0.0f}, {.2f, .2f, .2f}},
    };

//     std::vector<Model::Vertex> vertices{
//       // left face (white)
//       {{-.5f, -.5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.9f, .9f, .9f}},
//       {{-.5f,  .5f,  .5f}, {0.0f, 0.0f, 0.0f}, {.9f, .9f, .9f}},
//       {{-.5f, -.5f,  .5f}, {0.0f, 0.0f, 0.0f}, {.9f, .9f, .9f}},
//       {{-.5f, -.5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.9f, .9f, .9f}},
//       {{-.5f,  .5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.9f, .9f, .9f}},
//       {{-.5f,  .5f,  .5f}, {0.0f, 0.0f, 0.0f}, {.9f, .9f, .9f}},
 
//       // right face (yellow)
//       {{.5f, -.5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.8f, .8f, .1f}},
//       {{.5f, -.5f,  .5f}, {0.0f, 0.0f, 0.0f}, {.8f, .8f, .1f}},
//       {{.5f,  .5f,  .5f}, {0.0f, 0.0f, 0.0f}, {.8f, .8f, .1f}},
//       {{.5f, -.5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.8f, .8f, .1f}},
//       {{.5f,  .5f,  .5f}, {0.0f, 0.0f, 0.0f}, {.8f, .8f, .1f}},
//       {{.5f,  .5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.8f, .8f, .1f}},
 
//       // top face (orange, remember y axis points down)
//       {{-.5f, -.5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.9f, .6f, .1f}},
//       {{-.5f, -.5f,  .5f}, {0.0f, 0.0f, 0.0f}, {.9f, .6f, .1f}},
//       {{ .5f, -.5f,  .5f}, {0.0f, 0.0f, 0.0f}, {.9f, .6f, .1f}},
//       {{-.5f, -.5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.9f, .6f, .1f}},
//       {{ .5f, -.5f,  .5f}, {0.0f, 0.0f, 0.0f}, {.9f, .6f, .1f}},
//       {{ .5f, -.5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.9f, .6f, .1f}},
 
//       // bottom face (red)
//       {{-.5f, .5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.8f, .1f, .1f}},
//       {{ .5f, .5f,  .5f}, {0.0f, 0.0f, 0.0f}, {.8f, .1f, .1f}},
//       {{-.5f, .5f,  .5f}, {0.0f, 0.0f, 0.0f}, {.8f, .1f, .1f}},
//       {{-.5f, .5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.8f, .1f, .1f}},
//       {{ .5f, .5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.8f, .1f, .1f}},
//       {{ .5f, .5f,  .5f}, {0.0f, 0.0f, 0.0f}, {.8f, .1f, .1f}},
 
//       // nose face (blue)
//       {{-.5f, -.5f, 0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .1f, .8f}},
//       {{-.5f,  .5f, 0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .1f, .8f}},
//       {{ .5f,  .5f, 0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .1f, .8f}},
//       {{-.5f, -.5f, 0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .1f, .8f}},
//       {{ .5f,  .5f, 0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .1f, .8f}},
//       {{ .5f, -.5f, 0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .1f, .8f}},
 
//       // tail face (green)
//       {{-.5f, -.5f, -0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .8f, .1f}},
//       {{ .5f,  .5f, -0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .8f, .1f}},
//       {{-.5f,  .5f, -0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .8f, .1f}},
//       {{-.5f, -.5f, -0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .8f, .1f}},
//       {{ .5f, -.5f, -0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .8f, .1f}},
//       {{ .5f,  .5f, -0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .8f, .1f}},
//   };


    auto floor_obj = GameObject::create_game_object();
    floor_obj.model = new Model(_device, floor);

    _game_objects.push_back(std::move(floor_obj));


    auto cube_obj = GameObject::create_game_object();
    cube_obj.model = Model::load_from_file(_device, "models/colored_cube.obj");
    cube_obj.transform.translation = { 0.f, 0.f, 3.5f};
    cube_obj.transform.scale = { 0.5f, 0.5f, 0.5f};

    _game_objects.push_back(std::move(cube_obj));
}

}
