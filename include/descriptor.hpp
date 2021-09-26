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

private:

    constexpr bool initialized();
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
    std::cout << "binding: " << binding << '\n';
    std::cout << "buffers.size()" << buffers.size() << "\n";
    void* device_data;
    vkMapMemory(device->device(), buffers_memory[*current_index], 0, sizeof(T), 0, &device_data);
    memcpy(device_data, &data, sizeof(T));
    vkUnmapMemory(device->device(), buffers_memory[*current_index]);
}


template<typename T>
constexpr bool UniformBuffer<T>::initialized()
{
    return device != nullptr;
}

}

#endif // DESCRIPTOR_H