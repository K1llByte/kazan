// #include "test_app.hpp"

// int main()
// {
//     kzn::TestApp app{};
//     app.run();
//     return 0;
// }

/////////////////////////////////////////////////////


#include "device.hpp"

#include <iostream>

int main()
{
    kzn::Window window(800, 800, "Test Window");
    kzn::Device app(window);

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