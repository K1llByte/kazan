#ifndef KZN_WINDOW_HPP
#define KZN_WINDOW_HPP

#include "core/input.hpp"

#include <string_view>
#include <vector>

// FIXME: Useless
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// TODO: Implement Cheshire Cat window abstraction

namespace kzn
{
    class Input;

    class Window
    {
        friend void framebuffer_resized(GLFWwindow* , int, int);
    public:
        Window(const std::string_view& name, int width, int height);
        ~Window();

        bool should_close() const noexcept;
        void poll_events() const noexcept;
        void set_title(const std::string_view& name) noexcept;
        void set_resized(bool resized) noexcept;

        constexpr float aspect_ratio() const noexcept { return static_cast<float>(width) / static_cast<float>(height); }
        bool was_resized() noexcept;
        GLFWwindow* glfw_ptr() noexcept { return glfw_window; }
        Input input_handler() { return Input(glfw_window); }

        std::vector<const char*> required_extensions();
        VkExtent2D extent() noexcept;

        // Input related
        // TODO: Make an appropriate event system
        
    public:
        bool         has_resized = false;
    private:
        GLFWwindow*  glfw_window;
        int          width = 800;
        int          height = 600;

        VkSurfaceKHR surface = VK_NULL_HANDLE;
    };
}

#endif // KZN_WINDOW_HPP