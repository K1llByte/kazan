#ifndef TESTAPP_H
#define TESTAPP_H

#include "window.hpp"
#include "device.hpp"
#include "renderer.hpp"

namespace kzn
{

class TestApp
{
public:

    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

private:

    Window   _window;
    Instance _instance;
    Device   _device;
    std::unique_ptr<Renderer> _renderer = nullptr;

public:

    TestApp();
    ~TestApp();

    void run();

};

}

#endif // TESTAPP_H
