// #include "full_renderer.hpp"

#include "window.hpp"
#include "instance.hpp"

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

    kzn::Window window("Kazan Vulkan", 800, 800);
    auto instance = kzn::InstanceBuilder()
        .enable_extensions(window.required_extensions())
        .enable_validation_layers()
        .set_debug_messeger()
        .build();



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

    return EXIT_SUCCESS;
}