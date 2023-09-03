#include "dset.hpp"

#include "core/log.hpp"
#include "vk/utils.hpp"

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


DescriptorSet DescriptorSetAllocator::allocate(const VkDescriptorSetLayout& layout) {
    if(m_current_pool == VK_NULL_HANDLE) {
        m_current_pool = grab_pool();
        m_used_pools.push_back(m_current_pool);
    }

    VkDescriptorSetAllocateInfo alloc_info{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = m_current_pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &layout,
    };
    
    VkDescriptorSet set;
    auto result = vkAllocateDescriptorSets(m_device.vk_device(), &alloc_info, &set);

    if(result == VK_SUCCESS) [[likely]] {
        return DescriptorSet(m_device, set, layout);
    }
    else [[unlikely]] {
        if(result == VK_ERROR_FRAGMENTED_POOL
            || result == VK_ERROR_OUT_OF_POOL_MEMORY) [[likely]]
        {
            // Needs allocation of a
            // new pool and retry
            m_current_pool = grab_pool();
            m_used_pools.push_back(m_current_pool);

            result = vkAllocateDescriptorSets(m_device.vk_device(), &alloc_info, &set);
            VK_CHECK_MSG(result, "DescriptorSet allocation failed on new pool");
            return DescriptorSet(m_device, set, layout);
        }
    }

    throw ResultError(result);
}


VkDescriptorPool create_pool(
    Device&                                  device,
    const DescriptorSetAllocator::PoolSizes& pool_sizes,
    uint32_t                                 count,
    VkDescriptorPoolCreateFlags              flags)
{
    std::vector<VkDescriptorPoolSize> sizes;
    sizes.reserve(pool_sizes.size());
    for (auto sz : pool_sizes) {
        sizes.push_back({ sz.first, uint32_t(sz.second * count) });
    }
    
    VkDescriptorPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = flags,
        .maxSets = count,
        .poolSizeCount = static_cast<uint32_t>(sizes.size()),
        .pPoolSizes = sizes.data(),
    };

    VkDescriptorPool descriptor_pool;
    vkCreateDescriptorPool(device.vk_device(), &pool_info, nullptr, &descriptor_pool);

    return descriptor_pool;
}


VkDescriptorPool DescriptorSetAllocator::grab_pool() {
    if (m_free_pools.size() > 0) {
        VkDescriptorPool pool = m_free_pools.back();
        m_free_pools.pop_back();
        return pool;
    }
    else {
        return create_pool(m_device, m_descriptor_sizes, 1000, 0);
    }
}


DescriptorSet::DescriptorSet(
    Device&               device,
    VkDescriptorSet       descriptor_set,
    VkDescriptorSetLayout layout)
    : m_device{device}
    , m_vk_descriptor_set{descriptor_set}
    , m_layout{layout}
{
    
}

} // namespace kzn::vk
