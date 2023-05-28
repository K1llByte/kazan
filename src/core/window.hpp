#pragma once

#include <string_view>
#include <vector>

#include "vk/error.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vk/instance.hpp"
#include "vk/surface.hpp"

namespace kzn {

class Window {        
    friend void framebuffer_resized(GLFWwindow*, int, int);

public:
    // Ctor
    Window(const std::string_view& name, int width, int height);
    // Copy
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    // Move
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;
    // Dtor
    ~Window();

    bool is_closed() const;
    void poll_events() const;
    void set_title(const std::string_view& name);
    void set_resized(bool resized); // TODO: Remove when event system is implemented
    vk::Surface create_surface(vk::Instance& instance);

    VkExtent2D               extent();
    GLFWwindow*              glfw_ptr() const { return m_glfw_window; }
    bool                     was_resized(); // TODO: Remove when event system is implemented
    float                    aspect_ratio() const;
    std::vector<const char*> required_extensions();

private:
    GLFWwindow*  m_glfw_window;
    int          m_width = 800;
    int          m_height = 600;
    bool         m_has_resized; // TODO: Remove when event system is implemented
};

} // namespace kzn