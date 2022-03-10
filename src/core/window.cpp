#include "core/window.hpp"

#include "core/log.hpp"
#include "vk/utils.hpp"

namespace kzn
{
    Window::Window(const std::string_view& name, int width, int height)
    {
        // Initialize glfw
        glfwInit();
        // Turn off OpenGl context initialization
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // Turn off resizable window
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        // Initialize window
        glfw_window = glfwCreateWindow(width, height, name.data(), nullptr, nullptr);
        glfwSetWindowUserPointer(glfw_window, this);
        glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        // On framebuffer resize callback
        // glfwSetFramebufferSizeCallback(glfw_window, framebuffer_resize_callback);
        Log::debug("Window created");
    }

    Window::~Window()
    {
        // Destroy window
        glfwDestroyWindow(glfw_window);
        // Destroy glfw
        glfwTerminate();
        Log::debug("Window destroyed");
    }

    bool Window::should_close() const noexcept
    {
        return glfwWindowShouldClose(glfw_window);
    }

    void Window::poll_events() const noexcept
    {
        glfwPollEvents();
    }

    void Window::set_title(const std::string_view& name) noexcept
    {
        glfwSetWindowTitle(glfw_window, name.data());
    }

    std::vector<const char*> Window::required_extensions()
    {
        uint32_t glfw_extension_count = 0;
        const char** glfw_extensions;
        glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
        return std::vector<const char*>(
            glfw_extensions,
            glfw_extensions + glfw_extension_count);
    }

    VkExtent2D Window::extent() const noexcept
    {
        int width, height;
        glfwGetFramebufferSize(glfw_window, &width, &height);
        return VkExtent2D {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
    }
}