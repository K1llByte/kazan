// #include "window.hpp"
// #include "instance.hpp"
// #include "device.hpp"

#include "test_app.hpp"

#include <iostream>


int main()
{

    kzn::TestApp app;

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    //////////////////////////////////////////////////

    // kzn::Window window("Hello Kazan", 800, 600);

    // kzn::Instance instance{};

    // instance
    //     .enable_extensions(window.required_extensions())
    //     .enable_validation_layers()
    //     .set_debug_messeger()
    //     .init();

    // auto surface = window.create_surface(instance);


    // auto physical_device = kzn::PhysicalDeviceSelector(instance, surface)
    //     .select();

    // kzn::Device device(physical_device);
    // device.init();

    // window.destroy_surface(instance);

    return EXIT_SUCCESS;
}