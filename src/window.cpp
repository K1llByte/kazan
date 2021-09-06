#include "window.hpp"

#include <stdexcept>
#include <iostream>

namespace kzn
{

Window::Window(const std::string& name, int width, int height)
{
    // Initialize glfw
    glfwInit();

    // Turn off OpenGl context initialization
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    
    // Turn off resizable window
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    // Initialize window
    m_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

    // glfwSetWindowUserPointer(m_window, this);
    
    // On framebuffer resize callback
    // glfwSetFramebufferSizeCallback(m_window, framebuffer_resize_callback);

    std::cout << "+ Created window\n";
}

Window::~Window()
{
    // Destroy window
    glfwDestroyWindow(m_window);

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
    if(glfwCreateWindowSurface(instance.instance(), m_window, nullptr, &m_surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
    std::cout << "+ Surface created successfully\n";

    return m_surface;
}

void Window::destroy_surface(Instance& instance)
{
    // Destroy window surface
    vkDestroySurfaceKHR(instance.instance(), m_surface, nullptr);
    std::cout << "- Surface destroyed successfully\n";
}

}