#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include "device.hpp"
// #include "renderer.hpp"

namespace kzn
{

template<typename T>
class UniformBuffer
{
public:

    // friend class Renderer;
    
    Device*                     device = nullptr;
    int*                        current_index = nullptr;
    const uint32_t              binding;
    std::vector<VkBuffer>       buffers;
    std::vector<VkDeviceMemory> buffers_memory;

public:

    // UniformBuffer(const UniformBuffer&) = default;
    UniformBuffer();
    ~UniformBuffer();

    void operator=(const UniformBuffer<T>&);

    void update(const T& data);

private:

    bool initialized();
};

class DescriptorSet
{
public:

    // friend class Renderer;

    int*                         current_index;
    std::vector<VkDescriptorSet> descriptor_sets;

public:

    void bind(VkCommandBuffer command_buffer, VkPipelineLayout layout);
};

}

#endif // DESCRIPTOR_H