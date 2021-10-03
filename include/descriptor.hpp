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


template<typename T, size_t N>
class UniformBufferArray
{
public:

    // friend class Renderer;

    Device*                                   device = nullptr;
    uint32_t*                                 current_index = nullptr;
    uint32_t                                  binding;
    std::vector<VkBuffer>                     buffers;
    std::vector<VkDeviceMemory>               buffers_memory;

public:

    // UniformBuffer(const UniformBuffer&) = default;
    UniformBufferArray() = default;
    ~UniformBufferArray() = default;

    void update(const std::array<T,N>& data);
};


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

public:

    void bind(VkCommandBuffer command_buffer, VkPipelineLayout layout) const;
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


template<typename T, size_t N>
void UniformBufferArray<T,N>::update(const std::array<T,N>& data)
{
    void* device_data;
    vkMapMemory(device->device(), buffers_memory[*current_index], 0, sizeof(T), 0, &device_data);
    memcpy(device_data, &data, sizeof(T)*N);
    vkUnmapMemory(device->device(), buffers_memory[*current_index]);
}


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


// template<typename T>
// constexpr bool UniformBuffer<T>::initialized()
// {
//     return device != nullptr;
// }

}

#endif // DESCRIPTOR_H