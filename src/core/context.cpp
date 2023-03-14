#include "core/context.hpp"

#include "core/log.hpp"
#include "vk/device_features.hpp"

namespace kzn
{
    Context* Context::create(Window& window)
    {
        return (Context::self = new Context(window));
    }

    void Context::destroy()
    {
        delete self;
        self = nullptr;
    }

    Context::Context(Window& window)
        // Create Vulkan Instance
        : m_instance(vk::InstanceBuilder()
            .enable_validation_layers()
            .set_extensions(window.required_extensions())
            .build()),
        m_surface(m_instance.create_surface(window)),
        // Create Device
        m_device(vk::DeviceBuilder(m_instance)
            .set_surface(m_surface)
            // NOTE: IF THIS EXTENSION ISN'T LOADED THEN THE
            // SwapchainBuilder will seg fault
            .set_extensions({VK_KHR_SWAPCHAIN_EXTENSION_NAME})
            .set_features(vk::as_vk_device_features({
                vk::DeviceFeature::SamplerAnisotropy,
                vk::DeviceFeature::FillModeNonSolid
            }))
            .build()),
        // Create swapchain
        m_swapchain(vk::SwapchainBuilder(&m_device, m_surface, window.extent())
            .set_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            // .set_present_mode(VK_PRESENT_MODE_IMMEDIATE_KHR)
            .build()) {}
} // namespace kzn