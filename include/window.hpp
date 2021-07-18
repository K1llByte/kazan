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
    int width;
    int height;
    std::string name;
    GLFWwindow* window;
    
    void create();

    public:
    Window(int w, int h, std::string window_name);
    ~Window();

    // Avoid copy of this class
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool should_close();
};

}

#endif // WINDOW