#include "kazan.hpp"

#include <thread>
#include <functional>
#include <iostream>
#include <string_view>

using namespace kzn;


int main()
{
    auto window = Window("Hello World!", 800, 600);
    auto instance = vk::InstanceBuilder()
        .enable_validation_layers()
        .set_extensions(window.required_extensions())
        .build();
    auto surface = instance.create_surface(window.ptr());

    auto device = vk::DeviceBuilder(instance)
        .set_surface(surface)
        // NOTE: IF THIS EXTENSION ISN'T LOADED THEN THE SwapchainBuilder
        // will give a seg fault
        .set_extensions({VK_KHR_SWAPCHAIN_EXTENSION_NAME})
        .build();
    
    auto swapchain = vk::SwapchainBuilder(&device, surface, window.extent())
        .build();

    // TODO: Make VkPipelineLayout and VkRenderpass mandatory args
    auto pipeline_layout = vk::PipelineLayoutBuilder(&device)
        .build();
    // auto pipeline = vk::PipelineConfigBuilder()
    //     .set_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
    //     .set_polygon_mode(VK_POLYGON_MODE_FILL)
    //     .build();

    while(!window.should_close())
    {
        window.poll_events();
    }
    // instance.destroy_surface(surface);

    // List all devices if you want to choose the device
    // auto available_gpus = vk::Device::available_devices(instance);
    // Create default device
    // auto device = vk::Device::default();
}