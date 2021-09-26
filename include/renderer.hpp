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
    std::vector<VkBuffer>               _uniform_buffers;
    std::vector<VkDeviceMemory>         _uniform_buffers_memory;

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

////////////////////////////////////////////////////////////////

template<typename T>
UniformBuffer<T> Renderer::alloc_buffer(VkDescriptorSetLayoutBinding* layout_binding, VkShaderStageFlags stage)
{
    const size_t img_count = _swap_chain->image_count();

    UniformBuffer<T> ub;
    ub.device = _device;
    ub.buffers = std::vector<VkBuffer>(img_count);
    ub.buffers_memory = std::vector<VkDeviceMemory>(img_count);
    ub.current_index = &_current_image_index;
    ub.binding = _descriptor_buffer_infos.size() / img_count;
    VkDeviceSize buffer_size = sizeof(T);

    for(size_t i = 0; i < img_count; ++i)
    {
        _device->create_buffer(
            buffer_size,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, //| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            ub.buffers[i],
            ub.buffers_memory[i]);

        _uniform_buffers.push_back(ub.buffers[i]);
        _uniform_buffers_memory.push_back(ub.buffers_memory[i]);

        VkDescriptorBufferInfo buffer_info{};
        buffer_info.buffer = ub.buffers[i];
        buffer_info.offset = 0;
        buffer_info.range = sizeof(T);
        _descriptor_buffer_infos.push_back(buffer_info);
    }

    if(layout_binding != nullptr)
    {
        layout_binding->binding = ub.binding;
        layout_binding->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        layout_binding->descriptorCount = 1;
        layout_binding->stageFlags = stage; //VK_SHADER_STAGE_VERTEX_BIT;
        layout_binding->pImmutableSamplers = nullptr; // Optional
    }

    return ub;
}

}

#endif // RENDERER_H
