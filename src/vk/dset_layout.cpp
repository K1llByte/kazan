#include "dset_layout.hpp"

#include "vk/utils.hpp"

#include <algorithm>

namespace kzn::vk {

VkDescriptorSetLayoutBinding uniform_binding(uint32_t binding, VkShaderStageFlags stage_flags) {
    return VkDescriptorSetLayoutBinding{
        .binding = binding,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = stage_flags,
        .pImmutableSamplers = nullptr,
    };
}

VkDescriptorSetLayoutBinding sampler_binding(uint32_t binding, VkShaderStageFlags stage_flags) {
    return VkDescriptorSetLayoutBinding{
        .binding = binding,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .stageFlags = stage_flags,
        .pImmutableSamplers = nullptr,
    };
}


DescriptorSetLayoutCache::DescriptorSetLayoutCache(Device& device)
    : m_device{device}
{

}

DescriptorSetLayoutCache::~DescriptorSetLayoutCache() {
    for (auto& [_, layout] : m_layout_cache) {
        vkDestroyDescriptorSetLayout(m_device.vk_device(), layout, nullptr);
    }
}

VkDescriptorSetLayout DescriptorSetLayoutCache::create_layout(const DescriptorSetBindings& bindings) {
    DescriptorLayoutInfo layout_info{
        .bindings = bindings,
    };

    std::sort(
        layout_info.bindings.begin(),
        layout_info.bindings.end(),
        [](const auto& a, const auto& b ) {
            return a.binding < b.binding;
        }
    );

    auto it = m_layout_cache.find(layout_info);
    if (it != m_layout_cache.end()) {
        return it->second;
    }
    else {
        // Create vulkan layout
        VkDescriptorSetLayout layout;
        VkDescriptorSetLayoutCreateInfo layout_create_info{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(bindings.size()),
            .pBindings = bindings.data(),
        };
        auto result = vkCreateDescriptorSetLayout(m_device.vk_device(), &layout_create_info, nullptr, &layout);
        VK_CHECK_MSG(result, "Failed to create descriptor set layout");

        // Add layout to cache
        m_layout_cache[layout_info] = layout;
        
        auto it = m_layout_cache.find(layout_info);
        return layout;
    }
}

} // namespace kzn::vk
