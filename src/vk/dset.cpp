#include "dset.hpp"

#include "core/log.hpp"

namespace kzn::vk {

DescriptorSetAllocator::DescriptorSetAllocator(Device& device)
    : m_device(device)
    , m_descriptor_sizes{
        { VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.f },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.f },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.f },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
    }
{
    Log::trace("Created DSet Allocator!");
    m_used_pools.reserve(5);
    m_free_pools.reserve(5);
}


DescriptorSetAllocator::~DescriptorSetAllocator() {
    // Delete every pool held
    for(auto pool : m_free_pools) {
        vkDestroyDescriptorPool(m_device.vk_device(), pool, nullptr);
    }
    for(auto pool : m_used_pools) {
        vkDestroyDescriptorPool(m_device.vk_device(), pool, nullptr);
    }
    Log::trace("Destroyed DSet Allocator!");
}


void DescriptorSetAllocator::reset_pools() {
    for (auto& pool : m_used_pools) {
        vkResetDescriptorPool(m_device.vk_device(), pool, 0);
    }

    m_free_pools = std::move(m_used_pools);
    m_used_pools.clear();
    m_current_pool = VK_NULL_HANDLE;
}


// DescriptorSet DescriptorSetAllocator::allocate(const DescriptorSetLayout& layout) {

// }

} // namespace kzn::vk
