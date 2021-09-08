#include "window.hpp"

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
    
    // On framebuffer resize callback
    glfwSetFramebufferSizeCallback(_window, framebuffer_resize_callback);

    std::cout << "+ Created window\n";
}


Window::~Window()
{
    // Destroy window
    glfwDestroyWindow(_window);

    // Destroy glfw
    glfwTerminate();

    std::cout << "- Destroyed window\n";
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
    std::cout << "+ Surface created successfully\n";

    return _surface;
}


void Window::destroy_surface(Instance& instance)
{
    // Destroy window surface
    vkDestroySurfaceKHR(instance.instance(), _surface, nullptr);
    _surface = VK_NULL_HANDLE;
    std::cout << "- Surface destroyed successfully\n";
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


VkSurfaceKHR Window::surface() const
{
    return _surface;
}

}