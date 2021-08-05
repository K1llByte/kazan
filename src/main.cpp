// #include "test_app.hpp"
#include "renderer.hpp"

#include <iostream>

int main()
{
    // kzn::Window window(800, 800, "Test Window");
    // kzn::Device app(window);
    // kzn::TestApp app;

    kzn::Renderer app;

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}