#ifndef KZN_WINDOW_HPP
#define KZN_WINDOW_HPP

#include <string_view>
#include <vector>

#include "vk/error.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define KZN_WINDOW_GLFW_IMPL

#ifndef KZN_WINDOW_GLFW_IMPL
#   error "No Window implementation specified"
#endif

namespace kzn {
    class Window {        
        friend void framebuffer_resized(GLFWwindow*,int,int);

        public:
        Window(const std::string_view& name, int width, int height);
        ~Window();

        bool is_closed() const noexcept;
        void poll_events() const noexcept;
        void set_title(const std::string_view& name) noexcept;
        void set_resized(bool resized) noexcept;

        float aspect_ratio() const noexcept;
        std::vector<const char*> required_extensions();
        VkExtent2D extent() noexcept;
        bool was_resized() noexcept;
        // TODO: Make an appropriate event system
        
#ifdef KZN_WINDOW_GLFW_IMPL
        GLFWwindow* glfw_ptr() const noexcept { return glfw_window; }
#endif

        private:
#ifdef KZN_WINDOW_GLFW_IMPL
        GLFWwindow*  glfw_window;
#endif
        int          width = 800;
        int          height = 600;
        bool         has_resized;
    };
}

#endif // KZN_WINDOW_HPP