#ifndef KZN_WINDOW_HPP
#define KZN_WINDOW_HPP

#include "vk/instance.hpp"

#include <string_view>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace kzn
{
    class Window
    {
    public:
        Window(const std::string_view& name, int width, int height);
        ~Window();

        bool should_close() const noexcept;
        void poll_events() const noexcept;
        GLFWwindow* ptr() noexcept { return glfw_window; }

        std::vector<const char*> required_extensions();
        
    private:
        GLFWwindow*  glfw_window;
        int          width = 800;
        int          height = 600;

        VkSurfaceKHR surface = VK_NULL_HANDLE;
    };
}

#endif // KZN_WINDOW_HPP