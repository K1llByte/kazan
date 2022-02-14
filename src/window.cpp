#include "window.hpp"

#include "logger.hpp"

#include <stdexcept>
#include <iostream>

namespace kzn
{

Window::Window(const std::string& name, int win_width, int win_height)
    : width{win_width}, height{win_height}
{
    // Initialize glfw
    glfwInit();

    // Turn off OpenGl context initialization
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    
    // Turn off resizable window
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    // Initialize window
    _window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(_window, this);

    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    
    // On framebuffer resize callback
    glfwSetFramebufferSizeCallback(_window, framebuffer_resize_callback);

    Logger::debug("+ Created window");
}


Window::~Window()
{
    // Destroy window
    glfwDestroyWindow(_window);

    // Destroy glfw
    glfwTerminate();

    Logger::debug("- Destroyed window");
}


std::vector<const char*> Window::required_extensions()
{
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    return std::vector<const char*>(glfw_extensions, glfw_extensions + glfw_extension_count);
}


VkSurfaceKHR Window::create_surface(Instance& instance)
{
    // Create surface
    if(glfwCreateWindowSurface(instance.instance(), _window, nullptr, &_surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
    Logger::debug("+ Surface created successfully");

    return _surface;
}


void Window::destroy_surface(Instance& instance)
{
    // Destroy window surface
    vkDestroySurfaceKHR(instance.instance(), _surface, nullptr);
    _surface = VK_NULL_HANDLE;
    Logger::debug("- Surface destroyed successfully");
}


void Window::framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
    auto _window = reinterpret_cast<kzn::Window*>(glfwGetWindowUserPointer(window));
    _window->_framebuffer_resized = true;
    _window->width = width;
    _window->height = height;
}


bool Window::should_close()
{
    return glfwWindowShouldClose(_window);
}


bool Window::was_resized() const
{
    return _framebuffer_resized;
}


void Window::reset_resized_flag()
{
    _framebuffer_resized = false;
}


VkExtent2D Window::get_extent() const
{
    return VkExtent2D{
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };
}


std::array<double,2> Window::get_cursor_position() const
{
    double x, y;
    glfwGetCursorPos(_window, &x, &y);
    return { x, y };
}


void Window::set_cursor_position(double x, double y)
{
    glfwSetCursorPos(_window, x, y);
}



VkSurfaceKHR Window::surface() const
{
    return _surface;
}


GLFWwindow* Window::window()
{
    return _window;
}


int Window::get_key(int key_code)
{
    return glfwGetKey(_window, key_code);
}

}