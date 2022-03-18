#include "core/context.hpp"

#include "core/log.hpp"
#include "vk/device_features.hpp"

namespace kzn
{
    Context* Context::create(Window* window)
    {
        return (Context::self = new Context(window));
    }

    void Context::destroy()
    {
        delete self;
    }

    Context::Context(Window* window)
        // Create Vulkan Instance
        : _instance(vk::InstanceBuilder()
            .enable_validation_layers()
            .set_extensions(window->required_extensions())
            .build()),
        surface(_instance.create_surface(window->glfw_ptr())),
        // Create Device
        _device(vk::DeviceBuilder(_instance)
            .set_surface(surface)
            // NOTE: IF THIS EXTENSION ISN'T LOADED THEN THE
            // SwapchainBuilder will seg fault
            .set_extensions({VK_KHR_SWAPCHAIN_EXTENSION_NAME})
            .set_features(vk::as_vk_device_features({
                vk::DeviceFeature::SamplerAnisotropy
            }))
            .build()),
        // Create swapchain
        _swapchain(vk::SwapchainBuilder(&_device, surface, window->extent())
            .set_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .build()) {}
} // namespace kzn