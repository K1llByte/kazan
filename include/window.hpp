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
public:

    int width = 800;
    int height = 600;

private:

    GLFWwindow*    _window;
    VkSurfaceKHR   _surface = VK_NULL_HANDLE;

public:

    Window(const std::string& name, int width, int height);

    Window(const Window&) = delete;
    void operator=(const Window&) = delete;

    ~Window();

    std::vector<const char*> required_extensions();

    VkSurfaceKHR create_surface(Instance& instance);
    void destroy_surface(Instance& instance);

    static void framebuffer_resize_callback(GLFWwindow *window, int width, int height);

    bool should_close();
    VkExtent2D get_extent() const;
    VkSurfaceKHR surface() const;

// private:

};

} // namespace kzn

#endif // WINDOW