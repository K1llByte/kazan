#include "window.hpp"
#include "instance.hpp"
// #include "device.hpp"

#include <iostream>


// namespace tmp
// {
//     enum MsgSeverity
//     {
//         ERROR   = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
//         WARNING = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
//         INFO    = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
//         VERBOSE = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
//     };

//     void foo()
//     {
//         std::cout << "visible\n";
//     }
// }

int main()
{
    // {
    //     using namespace tmp;
    //     std::cout << MsgSeverity::ERROR << '\n';
    // }

    // tmp::foo();

    //////////////////////////////////////////////////

    // auto window = kzn::Window("Kazan Vulkan", 800, 800);

    // auto instance = kzn::InstanceBuilder()
    //     .enable_extensions(window.required_extensions())
    //     .enable_validation_layers()
    //     .set_debug_messeger()
    //     .build();

    // auto surface = window.create_surface(instance);
    
    // auto physical_device = kzn::PhysicalDeviceSelector(instance, surface)
    //     .select();

    // auto device = kzn::DeviceBuilder(physical_device)
    //     .build();

    // // ...

    // window.destroy_surface(instance);

    //////////////////////////////////////////////////

    // tmp::Renderer app;

    // try
    // {
    //     app.run();
    // }
    // catch (const std::exception &e)
    // {
    //     std::cerr << e.what() << std::endl;
    //     return EXIT_FAILURE;
    // }

    //////////////////////////////////////////////////

    // kzn::Engine app;

    // app.init();

	// app.run();

	// app.cleanup();

    //////////////////////////////////////////////////

    // kzn::Window window();

    // kzn::Instance instance()
    //     .enable_extensions(window.required_extensions())
    //     .enable_validation_layers()
    //     .set_debug_messeger()
    //     .init()

    // kzn::Device device(window);
    // device.init();

    kzn::Window window("Hello Kazan", 800, 600);

    
    // kzn::Instance intance = kzn::InstanceBuilder()
    //     .enable_extensions(window.required_extensions())
    //     .enable_validation_layers()
    //     .set_debug_messeger()
    //     .build();

    kzn::Instance instance{};

    instance
        .enable_extensions(window.required_extensions())
        .enable_validation_layers()
        .set_debug_messeger()
        .init();

    // std::cout << "size T: " << sizeof(kzn::Instance::TemporaryData) << '\n';
    // std::cout << "size T*: " << sizeof(kzn::Instance::TemporaryData*) << '\n';
    
    return EXIT_SUCCESS;
}