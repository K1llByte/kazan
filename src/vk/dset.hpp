#pragma once

#include "device.hpp"
// #include "vk/cmd_buffers.hpp"
// #include "core/log.hpp"

// #include <initializer_list>
// #include <unordered_map>
// #include <span>

namespace kzn::vk {

class DescriptorSet;
class DescriptorSetLayout;

///////////////////////////////////////////////////////////////////
//                   Descriptor Set Allocator                    //
///////////////////////////////////////////////////////////////////

// DescriptorSetAllocator is a manager of DescriptorSetPool's which creates new pools
// when its needed.
class DescriptorSetAllocator {
public:
    using PoolSizes = std::vector<std::pair<VkDescriptorType,float>>;

    // Ctor
    DescriptorSetAllocator(Device& device);
    // Copy
    DescriptorSetAllocator(const DescriptorSetAllocator&) = delete;
    DescriptorSetAllocator& operator=(const DescriptorSetAllocator&) = delete;
    // Move
    DescriptorSetAllocator(DescriptorSetAllocator&&) = delete;
    DescriptorSetAllocator& operator=(DescriptorSetAllocator&&) = delete;
    // Dtor
    ~DescriptorSetAllocator();

    void reset_pools();
    // DescriptorSet allocate(const DescriptorSetLayout& layout);

private:
    Device&                       m_device;
    VkDescriptorPool              m_current_pool = VK_NULL_HANDLE;
    PoolSizes                     m_descriptor_sizes;
    std::vector<VkDescriptorPool> m_used_pools;
    std::vector<VkDescriptorPool> m_free_pools;

private:
    VkDescriptorPool grab_pool();
};

} // namespace kzn::vk