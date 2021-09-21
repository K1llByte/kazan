#ifndef UI_H
#define UI_H

#include "window.hpp"
#include "instance.hpp"
#include "device.hpp"
#include "renderer.hpp"

namespace kzn
{

class Interface
{
private:

    bool             initialized = false;
    bool             render_ui = false;
    VkDevice         _device = VK_NULL_HANDLE;
    VkDescriptorPool imgui_pool;

public:

    Interface() = default;
    Interface(
        Window& window,
        Instance& instance,
        Device& device,
        Renderer& renderer);
    // ~Interface();

    void cleanup();

    void toggle();

    void render();
    void draw(VkCommandBuffer cmd_buffer);
};

}

#endif // UI_H