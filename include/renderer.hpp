#ifndef RENDERER_H
#define RENDERER_H

#include "window.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include "descriptor.hpp"

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

    VkDescriptorPool                    _descriptor_pool = VK_NULL_HANDLE;
    std::vector<VkDescriptorBufferInfo> _descriptor_buffer_infos;
    std::vector<VkDescriptorSet>        _descriptor_sets;

    uint32_t _current_image_index;
    int      _current_frame_index = 0;
    bool     _frame_started = false;

public:

    Renderer() = default;
    Renderer(Window* window, Device* device);
    ~Renderer();

    void init();
    void cleanup();
    bool initialized() const;

    VkRenderPass render_pass() const;
    float aspect_ratio() const;

    void recreate_swap_chain();
    void create_command_buffers();
    
    void init_descriptor_pool(VkDescriptorSetLayout descriptor_set_layout);
    template<typename T>
    UniformBuffer<T> alloc_buffer(
        VkDescriptorSetLayoutBinding* layout_binding = nullptr,
        VkShaderStageFlags stage = VK_SHADER_STAGE_ALL_GRAPHICS);
    DescriptorSet init_descriptor_set();

    float delta_time();

    VkCommandBuffer begin_frame();
    void end_frame();
    void begin_render_pass(VkCommandBuffer command_buffer);
    void end_render_pass(VkCommandBuffer command_buffer);
};

}

#endif // RENDERER_H
