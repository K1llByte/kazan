#include "descriptor.hpp"

namespace kzn
{

// template<typename T>
// UniformBuffer<T>::UniformBuffer()
// {

// }


// template<typename T>
// UniformBuffer<T>::~UniformBuffer()
// {
//     if(initialized())
//     {
//         for(size_t i = 0; i < buffers.size(); ++i)
//         {
//             vkDestroyBuffer(device->device(), buffers[i], nullptr);
//             vkFreeMemory(device->device(), buffers_memory[i], nullptr);
//         }
//     }
// }


// template<typename T>
// UniformBuffer<T>& UniformBuffer<T>::operator=(const UniformBuffer<T>& other)
// {
//     device = other.device;
//     current_index = other.current_index;
//     binding = other.binding;
//     buffers = other.buffers;
//     buffers_memory = other.buffers_memory;

//     return *this;
// }


// template<typename T>
// void UniformBuffer<T>::update(const T& data)
// {
//     void* device_data;
//     vkMapMemory(device->device(), buffers_memory[*current_index], 0, sizeof(T), 0, &device_data);
//     memcpy(device_data, &data, sizeof(T));
//     vkUnmapMemory(device->device(), buffers_memory[*current_index]);
// }


// template<typename T>
// bool UniformBuffer<T>::initialized()
// {
//     return device != nullptr;
// }


// void DescriptorSet::bind(VkCommandBuffer command_buffer, VkPipelineLayout layout)
// {
//     vkCmdBindDescriptorSets(
//         command_buffer,
//         VK_PIPELINE_BIND_POINT_GRAPHICS,
//         layout,
//         0,
//         1,
//         &descriptor_sets[*current_index],
//         0,
//         nullptr);
// }

}
