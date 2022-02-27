#include "kazan.hpp"

#include <thread>
#include <functional>
// #include <chrono>

using namespace kzn;

int main()
{
    // Log::info("Hello World!");
    auto window = Window("Hello World!", 800, 600);
    auto instance = vk::InstanceBuilder()
        .enable_validation_layers()
        .set_extensions(window.required_extensions())
        .build();
    auto surface = instance.create_surface(window.ptr());

    auto device = vk::DeviceBuilder(instance)
        .set_surface(surface)
        .build();

    while(!window.should_close())
    {
        window.poll_events();
    }
    instance.destroy_surface(surface);

    // List all devices if you want to choose the device
    // auto available_gpus = vk::Device::available_devices(instance);
    // Create default device
    // auto device = vk::Device::default();
}