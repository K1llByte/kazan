#ifndef RENDERER_H
#define RENDERER_H

#include "window.hpp"
#include "device.hpp"
#include "swap_chain.hpp"

#include <vector>

namespace kzn
{

class Renderer
{

public:

    Window*                      _window = nullptr;
    Device*                      _device = nullptr;
    SwapChain*                   _swap_chain = nullptr;
    std::vector<VkCommandBuffer> _command_buffers;

    uint32_t _current_image_index;
    int      _current_frame_index = 0;
    bool     _frame_started = false;

public:

    Renderer(Window* window, Device* device);
    ~Renderer();

    void init();
    void cleanup();
    bool initialized() const;

    VkRenderPass render_pass() const;
    float aspect_ratio() const;

    void recreate_swap_chain();
    void create_command_buffers();

    VkCommandBuffer begin_frame();
    void end_frame();
    void begin_render_pass(VkCommandBuffer command_buffer);
    void end_render_pass(VkCommandBuffer command_buffer);
};

}

#endif // RENDERER_H
