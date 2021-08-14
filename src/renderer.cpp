#include "renderer.hpp"

namespace kzn
{

inline Instance&& init_instance(Window& window)
{
    return std::move(InstanceBuilder()
        .enable_extensions(window.required_extensions())
        .enable_validation_layers()
        .set_debug_messeger()
        .build());
}

inline Device&& init_device(Instance& instance, Window& window)
{
    auto surface = window.create_surface(instance);
    
    auto physical_device = PhysicalDeviceSelector(instance, surface)
        .select();

    return std::move(DeviceBuilder(physical_device)
        .build());
}


Renderer::Renderer()
    : m_window("Kazan Vulkan", 800, 800),
    m_instance{init_instance(m_window)},
    m_device{init_device(m_instance, m_window)}
{
    // m_window = Window("Kazan Vulkan", 800, 800);

    // m_instance = InstanceBuilder()
    //     .enable_extensions(m_window.required_extensions())
    //     .enable_validation_layers()
    //     .set_debug_messeger()
    //     .build();

    // auto surface = m_window.create_surface(m_instance);
    
    // auto physical_device = PhysicalDeviceSelector(m_instance, surface)
    //     .select();

    // m_device = DeviceBuilder(physical_device)
    //     .build();
}

Renderer::~Renderer()
{
    m_window.destroy_surface(m_instance);
}


void Renderer::run()
{

}

}