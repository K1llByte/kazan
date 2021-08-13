#ifndef WINDOW
#define WINDOW

#include "instance.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

namespace kzn
{

class Window
{
private:

    GLFWwindow* m_window;
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;

public:

    Window(const std::string& name, int width, int height);

    Window(const Window&) = delete;
    void operator=(const Window&) = delete;

    ~Window();

    std::vector<const char*> required_extensions();

    VkSurfaceKHR create_surface(Instance& instance);

    void destroy_surface(Instance& instance);

private:

};

} // namespace kzn

#endif // WINDOW