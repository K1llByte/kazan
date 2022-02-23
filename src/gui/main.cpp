#include "kazan.hpp"

#include <thread>
#include <functional>
// #include <chrono>

using namespace kzn;

int main()
{
    // Log::info("Hello World!");
    
    auto instance = vk::InstanceBuilder()
        .enable_validation_layers()
        .build();

    auto physical_device = vk::PhysicalDeviceSelector(instance).select();
}