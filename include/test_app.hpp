#ifndef TEST_APP
#define TEST_APP

#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"

namespace kzn
{

class TestApp
{
    private:
    Window window;
    Device device;
    Pipeline pipeline;

    public:
    static const int WIDTH = 800;
    static const int HEIGHT = 800;

    TestApp();
    ~TestApp() = default;
    void run();
};

}

#endif // TEST_APP