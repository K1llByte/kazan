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


bool DescriptorSetLayoutCache::DescriptorLayoutInfo::operator==(const DescriptorLayoutInfo& other) const {
    if (other.bindings.size() != bindings.size()) {
        return false;
    }
    else {
        // Compare each of the bindings is the same. Bindings are sorted so they will match
        for(size_t i = 0; i < bindings.size(); i++) {
            if(other.bindings[i].binding != bindings[i].binding
                || other.bindings[i].descriptorType != bindings[i].descriptorType
                || other.bindings[i].descriptorCount != bindings[i].descriptorCount
                || other.bindings[i].stageFlags != bindings[i].stageFlags)
            {
                return false;
            }
        }
        return true;
    }
}


size_t DescriptorSetLayoutCache::DescriptorLayoutInfo::hash() const
{
    size_t result = std::hash<std::size_t>()(bindings.size());

    for(const auto& b : bindings) {
        // Pack the binding data into a single int64. Not fully correct but its ok
        size_t binding_hash = b.binding
            | b.descriptorType << 8
            | b.descriptorCount << 16
            | b.stageFlags << 24;

        // Shuffle the packed binding data and xor it with the main hash
        result ^= std::hash<size_t>()(binding_hash);
    }

    return result;
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

DescriptorSetLayout DescriptorSetLayoutCache::create_layout(const DescriptorSetBindings& bindings) {
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
        return DescriptorSetLayout(
            DescriptorSetBindings(it->first.bindings),
            it->second
        );
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
        const auto [it, _] = m_layout_cache.insert({ layout_info, layout });
        
        return DescriptorSetLayout(
            DescriptorSetBindings(it->first.bindings),
            layout
        );
    }
}


DescriptorSetLayout::DescriptorSetLayout(
    DescriptorSetBindings&& layout_bindings,
    VkDescriptorSetLayout layout)
    : m_layout_bindings{std::move(layout_bindings)}
    , m_layout{layout}
{

}

} // namespace kzn::vk
