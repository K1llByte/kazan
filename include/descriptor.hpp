#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include "device.hpp"
// #include "renderer.hpp"

#include <cstring>

// Debug
#include <iostream>

namespace kzn
{

// template<typename T>
// class UniformBuffer
// {
// public:

//     // friend class Renderer;

//     Device*                     device = nullptr;
//     int*                        current_index = nullptr;
//     const uint32_t              binding;
//     std::vector<VkBuffer>       buffers;
//     std::vector<VkDeviceMemory> buffers_memory;

// public:

//     // UniformBuffer(const UniformBuffer&) = default;
//     UniformBuffer();
//     ~UniformBuffer();

//     UniformBuffer<T>& operator=(const UniformBuffer<T>&);

//     void update(const T& data);

// private:

//     bool initialized();
// };

// class DescriptorSet
// {
// public:

//     // friend class Renderer;

//     int*                         current_index;
//     std::vector<VkDescriptorSet> descriptor_sets;

// public:

//     void bind(VkCommandBuffer command_buffer, VkPipelineLayout layout);
// };

////////////////////////////////////////////////////////////////

template<typename T>
class UniformBuffer
{
public:

    // friend class Renderer;

    Device*                     device = nullptr;
    uint32_t*                   current_index = nullptr;
    uint32_t                    binding;
    std::vector<VkBuffer>       buffers;
    std::vector<VkDeviceMemory> buffers_memory;

public:

    // UniformBuffer(const UniformBuffer&) = default;
    UniformBuffer() = default;
    ~UniformBuffer() = default;

    void update(const T& data);
};


// template<typename T, size_t N>
// class UniformBufferArray
// {
// public:

//     // friend class Renderer;

//     Device*                                   device = nullptr;
//     uint32_t*                                 current_index = nullptr;
//     uint32_t                                  binding;
//     std::vector<VkBuffer>                     buffers;
//     std::vector<VkDeviceMemory>               buffers_memory;

// public:

//     // UniformBuffer(const UniformBuffer&) = default;
//     UniformBufferArray() = default;
//     ~UniformBufferArray() = default;

//     void update(const std::array<T,N>& data);
// };


template<typename T>
class PushConstant
{
public:

    // friend class Renderer;

    // T                  _data;
    VkShaderStageFlags stages;

public:

    // PushConstant(const PushConstant&) = default;
    PushConstant() = default;
    ~PushConstant() = default;

    void push(VkCommandBuffer command_buffer, VkPipelineLayout layout, const T& data);
};


class DescriptorSet
{
public:

    // friend class Renderer;

    uint32_t*                    current_index;
    std::vector<VkDescriptorSet> descriptor_sets;
    VkDescriptorSetLayout        descriptor_set_layout;

public:

    void bind(VkCommandBuffer command_buffer, VkPipelineLayout layout) const;
};


class DescriptorSetBuilder
{
public:

    Device*                             device;
    VkDescriptorPool                    pool;
    uint32_t*                           current_index;
    size_t                              image_count;

    std::vector<VkDescriptorBufferInfo>       descriptor_buffer_infos;
    std::vector<VkDescriptorSetLayoutBinding> layout_bindings;
    VkDescriptorSetLayout                     descriptor_set_layout;
    
public:

    template<typename T>
    UniformBuffer<T> create_uniform_buffer(VkShaderStageFlags stage = VK_SHADER_STAGE_ALL_GRAPHICS);

    DescriptorSet build();
};


class DescriptorPool
{
public:

    // friend class Renderer;

    Device*          device;
    VkDescriptorPool pool;
    uint32_t*        current_index;
    size_t           image_count;

public:

    DescriptorSetBuilder descriptor_set_builder();
};

////////////////////////////////////////////////////////////////

template<typename T>
void UniformBuffer<T>::update(const T& data)
{
    void* device_data;
    vkMapMemory(device->device(), buffers_memory[*current_index], 0, sizeof(T), 0, &device_data);
    memcpy(device_data, &data, sizeof(T));
    vkUnmapMemory(device->device(), buffers_memory[*current_index]);
}


// template<typename T, size_t N>
// void UniformBufferArray<T,N>::update(const std::array<T,N>& data)
// {
//     void* device_data;
//     vkMapMemory(device->device(), buffers_memory[*current_index], 0, sizeof(T), 0, &device_data);
//     memcpy(device_data, &data, sizeof(T)*N);
//     vkUnmapMemory(device->device(), buffers_memory[*current_index]);
// }


template<typename T>
void PushConstant<T>::push(VkCommandBuffer command_buffer, VkPipelineLayout layout, const T& data)
{
    vkCmdPushConstants(
        command_buffer,
        layout,
        stages,
        0,
        sizeof(T),
        &data);
}


template<typename T>
UniformBuffer<T> DescriptorSetBuilder::create_uniform_buffer(VkShaderStageFlags stage)
{
    
    UniformBuffer<T> ub;
    ub.device = this->device;
    ub.buffers = std::vector<VkBuffer>(image_count);
    ub.buffers_memory = std::vector<VkDeviceMemory>(image_count);
    ub.current_index = current_index;
    ub.binding = static_cast<uint32_t>(descriptor_buffer_infos.size()) / image_count;
    VkDeviceSize buffer_size = sizeof(T);

    for(size_t i = 0; i < image_count; ++i)
    {
        device->create_buffer(
            buffer_size,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, //| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            ub.buffers[i],
            ub.buffers_memory[i]);

        // _uniform_buffers.push_back(ub.buffers[i]);
        // _uniform_buffers_memory.push_back(ub.buffers_memory[i]);

        // VkDescriptorBufferInfo buffer_info{};
        // buffer_info.buffer = ub.buffers[i];
        // buffer_info.offset = 0;
        // buffer_info.range = sizeof(T);
        // descriptor_buffer_infos.push_back(buffer_info);

        descriptor_buffer_infos.push_back(
            VkDescriptorBufferInfo{
                .buffer = ub.buffers[i],
                .offset = 0,
                .range  = sizeof(T)
            }
        );
    }

    layout_bindings.push_back({
        .binding =            ub.binding,
        .descriptorType =     VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount =    1,
        .stageFlags =         stage,
        .pImmutableSamplers = nullptr
    });

    return ub;
}


// template<typename T>
// constexpr bool UniformBuffer<T>::initialized()
// {
//     return device != nullptr;
// }

}

#endif // DESCRIPTOR_H