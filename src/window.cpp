#include "window.hpp"

namespace kzn
{

Window::Window(int w, int h, std::string window_name)
    : width{w}, height{h}, name{window_name}
{
    create();
}

Window::~Window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::create()
{
    // Initialize glfw
    glfwInit();
    // Turn off OpenGl context initialization
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // Turn off resizable window
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
}

bool Window::should_close()
{
    return glfwWindowShouldClose(window);
}

}