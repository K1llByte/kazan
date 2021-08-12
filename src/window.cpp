#include "window.hpp"


namespace kzn
{

Window::Window(const std::string& name, int width, int height)
{
    // Initialize glfw
    glfwInit();
    // Turn off OpenGl context initialization
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    
    // Turn off resizable window
    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    // Initialize window
    m_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

    // glfwSetWindowUserPointer(m_window, this);
    
    // On framebuffer resize callback
    // glfwSetFramebufferSizeCallback(m_window, framebuffer_resize_callback);
}

Window::~Window()
{
    // // Destroy window surface
    // vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

    // Destroy window
    glfwDestroyWindow(m_window);

    // Destroy glfw
    glfwTerminate();
}

std::vector<const char*> Window::required_extensions()
{
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    return std::vector<const char*>(glfw_extensions, glfw_extensions + glfw_extension_count);
}

}