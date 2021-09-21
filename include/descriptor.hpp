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
    int*                        current_index = nullptr;
    uint32_t                    binding;
    std::vector<VkBuffer>       buffers;
    std::vector<VkDeviceMemory> buffers_memory;

public:

    // UniformBuffer(const UniformBuffer&) = default;
    UniformBuffer() {}
    ~UniformBuffer()
    {
        // if(initialized())
        // {
        //     std::cout << "Deleted Uniform Buffer\n";
        //     for(size_t i = 0; i < buffers.size(); ++i)
        //     {
        //         vkDestroyBuffer(device->device(), buffers[i], nullptr);
        //         vkFreeMemory(device->device(), buffers_memory[i], nullptr);
        //     }
        // }
    }


    // UniformBuffer<T>& operator=(UniformBuffer<T>&& other)
    // {
    //     std::cout << "Called move operator=\n";
    //     device = other.device;
    //     current_index = other.current_index;
    //     binding = other.binding;
    //     buffers = other.buffers;
    //     buffers_memory = other.buffers_memory;

    //     // This is to avoid destruction of buffers
    //     other.device = nullptr;

    //     return *this;
    // }

    void update(const T& data)
    {
        void* device_data;
        vkMapMemory(device->device(), buffers_memory[*current_index], 0, sizeof(T), 0, &device_data);
        memcpy(device_data, &data, sizeof(T));
        vkUnmapMemory(device->device(), buffers_memory[*current_index]);
    }

private:

    bool initialized()
    { return device != nullptr; }
};

class DescriptorSet
{
public:

    // friend class Renderer;

    // int*                         current_index;
    uint32_t*                    current_index;
    std::vector<VkDescriptorSet> descriptor_sets;

public:

    void bind(VkCommandBuffer command_buffer, VkPipelineLayout layout)
    {
        std::cout << "bind: #" << *current_index << "\n";
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            layout,
            0,
            1,
            &descriptor_sets[*current_index],
            0,
            nullptr);
    }
};

}

#endif // DESCRIPTOR_H