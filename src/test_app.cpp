#include "test_app.hpp"

#include "simple_render_system.hpp"

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

    _renderer = std::make_unique<Renderer>(_window, _device);

    // After engine initialization
    load_game_objects();
}


TestApp::~TestApp()
{
    _renderer.reset();
    _window.destroy_surface(_instance);
}


void TestApp::run()
{
    SimpleRenderSystem render_system{_device, _renderer->render_pass()};

    while(!_window.should_close())
    {
        glfwPollEvents();

        if(auto command_buffer = _renderer->begin_frame())
        {
            _renderer->begin_render_pass(command_buffer);
            render_system.render_game_objects(command_buffer, _game_objects);
            _renderer->end_render_pass(command_buffer);
            _renderer->end_frame();
        }
    }

    _device.wait_idle();
}


void TestApp::load_game_objects()
{
    std::vector<Model::Vertex> vertices{
        {{ 0.0f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
        };
    auto model = new Model(_device, vertices);

    auto triangle = GameObject::create_game_object();
    triangle.model = model;

    _game_objects.push_back(std::move(triangle));
}

}
