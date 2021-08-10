#ifndef WINDOW
#define WINDOW

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace kzn
{

class Window
{
private:

    GLFWwindow* m_window;
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    // VkSurfaceKHR m_surface;

public:

    Window(const std::string& name, int width, int height);

    Window(const Window&) = delete;
    void operator=(const Window&) = delete;

    ~Window();

private:

    void init_window();
};

} // kzn

#endif // WINDOW