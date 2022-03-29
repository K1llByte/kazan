#include "core/window.hpp"

#include "core/log.hpp"
#include "vk/utils.hpp"

namespace kzn
{
    void framebuffer_resized(GLFWwindow* window, int width, int height)
    {
        auto app = reinterpret_cast<kzn::Window*>(glfwGetWindowUserPointer(window));
        app->has_resized = true;
        app->width = width;
        app->height = height;
    }

    Window::Window(const std::string_view& name, int width, int height)
    {
        // Initialize glfw
        glfwInit();
        // Turn off OpenGl context initialization
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // Turn off resizable window
        // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        // Initialize window
        glfw_window = glfwCreateWindow(width, height, name.data(), nullptr, nullptr);
        glfwSetWindowUserPointer(glfw_window, this);
        // Hide mouse cursor
        // glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        // On framebuffer resize callback
        glfwSetFramebufferSizeCallback(glfw_window, framebuffer_resized);
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

    void Window::set_resized(bool resized) noexcept
    {
        has_resized = resized;
    }

    bool Window::was_resized() noexcept
    {
        const auto was_resized = has_resized;
        has_resized = false;
        return was_resized;
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

    VkExtent2D Window::extent() noexcept
    {
        // glfwGetFramebufferSize(glfw_window, &width, &height);
        // Special case for minimized window
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(glfw_window, &width, &height);
            glfwWaitEvents();
        }
        return VkExtent2D {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
    }
}