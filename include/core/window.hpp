#ifndef KZN_WINDOW_HPP
#define KZN_WINDOW_HPP

#include <string_view>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace kzn
{
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

        constexpr float aspect_ratio() const noexcept { return width / height; }
        bool was_resized() noexcept;
        GLFWwindow* glfw_ptr() noexcept { return glfw_window; }

        std::vector<const char*> required_extensions();
        VkExtent2D extent() noexcept;
        
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