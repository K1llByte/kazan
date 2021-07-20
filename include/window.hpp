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
    int m_width;
    int m_height;
    std::string m_name;
    GLFWwindow* m_window;
    
    void create();

    public:
    Window(int w, int h, std::string window_name);
    ~Window();

    // Avoid copy of this class
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool should_close();
    void create_window_surface(VkInstance instance, VkSurfaceKHR* surface);
};

}

#endif // WINDOW