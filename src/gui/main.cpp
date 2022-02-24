#include "kazan.hpp"

#include <thread>
#include <functional>
// #include <chrono>

using namespace kzn;

// TODO:
// - Window

int main()
{
    // Log::info("Hello World!");
    
    auto instance = vk::InstanceBuilder()
        .enable_validation_layers()
        .build();

    auto device = vk::DeviceBuilder(instance)
        .build();

    // List all devices if you want to choose the device
    // auto available_gpus = vk::Device::available_devices(instance);
}