#include "window.hpp"

#include "log.hpp"
// #include "vk/utils.hpp"

namespace kzn {

void framebuffer_resized(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    app->m_has_resized = true;
    app->m_width = width;
    app->m_height = height;
}

Window::Window(const std::string_view& name, int _width, int _height)
    : m_width(_width)
    , m_height(_height) {
    // Initialize glfw
    glfwInit();
    // Turn off OpenGL context initialization
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Turn off resizable window
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    // Create window
    m_glfw_window =
        glfwCreateWindow(m_width, m_height, name.data(), nullptr, nullptr);
    glfwSetWindowUserPointer(m_glfw_window, this);

    // On framebuffer resize callback
    glfwSetFramebufferSizeCallback(m_glfw_window, framebuffer_resized);

    // Hide mouse cursor
    // glfwSetInputMode(m_glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    Log::trace("Window created");
}

Window::~Window() {
    // Destroy window
    glfwDestroyWindow(m_glfw_window);
    // Destroy glfw
    glfwTerminate();
    m_glfw_window = nullptr;
    Log::trace("Window destroyed");
}

bool Window::is_closed() const {
    return glfwWindowShouldClose(m_glfw_window);
}

void Window::poll_events() const {
    glfwPollEvents();
}

//! FIXME: This crashes the engine
void Window::set_title(const std::string_view& name) {
    glfwSetWindowTitle(m_glfw_window, name.data());
}

void Window::set_resized(bool resized) {
    m_has_resized = resized;
}

vk::Surface Window::create_surface(vk::Instance& instance) {
    VkSurfaceKHR surface;
    auto result = glfwCreateWindowSurface(
        instance.vk_instance(), m_glfw_window, nullptr, &surface
    );
    VK_CHECK_MSG(result, "Failed to create window surface!");
    return vk::Surface(instance, surface);
}

float Window::aspect_ratio() const {
    return static_cast<float>(m_width) / static_cast<float>(m_height);
}

std::vector<const char*> Window::required_extensions() const {
    uint32_t glfw_extension_count = 0;
    // const char** glfw_extensions;
    auto glfw_extensions =
        glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    return { glfw_extensions, glfw_extensions + glfw_extension_count };
}

VkExtent2D Window::extent() {
    // glfwGetFramebufferSize(m_glfw_window, &m_width, &m_height);
    // Special case for minimized window
    while (m_width == 0 || m_height == 0) {
        glfwGetFramebufferSize(m_glfw_window, &m_width, &m_height);
        glfwWaitEvents();
    }
    return VkExtent2D{ static_cast<uint32_t>(m_width),
                       static_cast<uint32_t>(m_height) };
}

bool Window::was_resized() {
    const bool was_resized = m_has_resized;
    m_has_resized = false;
    return was_resized;
}

} // namespace kzn