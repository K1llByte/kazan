#include "test_app.hpp"

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
}


TestApp::~TestApp()
{
    _window.destroy_surface(_instance);
}


void TestApp::run()
{
    bool torf = true;
    while (!_window.should_close())
    {
        glfwPollEvents();
        if(torf)
        {
            torf = false;
            std::cout << "In cycle\n";
        }
    }

    _device.wait_idle();
}

}
