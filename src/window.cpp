#include "window.hpp"

#include <stdexcept>

namespace kzn
{

Window::Window(int w, int h, std::string window_name)
    : m_width{w}, m_height{h}, m_name{window_name}
{
    create();
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
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

    m_window = glfwCreateWindow(m_width, m_height, m_name.c_str(), nullptr, nullptr);
}

bool Window::should_close()
{
    return glfwWindowShouldClose(m_window);
}

void Window::create_window_surface(VkInstance instance, VkSurfaceKHR* surface)
{
    if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS)
            throw std::runtime_error("failed to create window surface!");
}

}