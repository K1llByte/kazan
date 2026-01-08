#pragma once

#include <string_view>
#include <vector>

#include "vk/error.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vk/instance.hpp"
#include "vk/surface.hpp"

namespace kzn {

class Input;

class Window {
    friend void framebuffer_resized(GLFWwindow*, int, int);

public:
    // Ctor
    Window(std::string_view name, int width, int height);
    // Copy
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    // Move
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;
    // Dtor
    ~Window();

    [[nodiscard]]
    bool is_closed() const;

    void poll_events() const;

    void set_title(std::string_view name);

    // TODO: Remove when event system is implemented
    void set_resized(bool resized);

    vk::Surface create_surface(vk::Instance& instance);

    VkExtent2D extent();
    [[nodiscard]]
    GLFWwindow* glfw_ptr() const {
        return m_glfw_window;
    }
    // TODO: Remove when event system is implemented
    [[nodiscard]]
    bool was_resized();
    [[nodiscard]]
    float aspect_ratio() const;
    [[nodiscard]]
    std::vector<const char*> required_extensions() const;

private:
    friend class Input;

    [[nodiscard]]
    Input* input() {
        return m_input_ptr;
    }

    void set_input(Input* input_ptr) { m_input_ptr = input_ptr; }

private:
    GLFWwindow* m_glfw_window;
    int m_width = 800;
    int m_height = 600;
    bool m_has_resized; // TODO: Remove when event system is implemented
    Input* m_input_ptr = nullptr;
};

} // namespace kzn