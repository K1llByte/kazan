#ifndef RENDERER
#define RENDERER

#include "window.hpp"
#include "instance.hpp"
#include "device.hpp"

// #include <iostream>

namespace kzn
{

class Renderer
{
private:

    Window   m_window;
    Instance m_instance;
    Device   m_device;

public:

    Renderer();

    ~Renderer();

    void run();

};

} // namespace kzn

#endif // RENDERER