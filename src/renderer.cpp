#include "renderer.hpp"

namespace kzn
{

// Instance init_instance(Window& window)
// {
//     return InstanceBuilder()
//         .enable_extensions(window.required_extensions())
//         .enable_validation_layers()
//         .set_debug_messeger()
//         .build();
// }

// Instance init_physical_device(Instance& instance, Window& window)
// {
//     auto surface = window.create_surface(instance);
    
//     auto physical_device = PhysicalDeviceSelector(instance, surface)
//         .select();

//     return DeviceBuilder(physical_device)
//         .build();
// }


Renderer::Renderer()
    // : m_window("Kazan Vulkan", 800, 800),
{
    m_window = Window("Kazan Vulkan", 800, 800);

    m_instance = InstanceBuilder()
        .enable_extensions(m_window.required_extensions())
        .enable_validation_layers()
        .set_debug_messeger()
        .build();

    auto surface = m_window.create_surface(m_instance);
    
    auto physical_device = PhysicalDeviceSelector(m_instance, surface)
        .select();

    m_device = DeviceBuilder(physical_device)
        .build();
}

Renderer::~Renderer()
{
    m_window.destroy_surface(m_instance);
}


void Renderer::run()
{

}

}