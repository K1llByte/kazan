#include "dset_layout.hpp"

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

}

VkDescriptorSetLayout create_layout(const DescriptorSetBindings& info) {
    return VK_NULL_HANDLE;
}

} // namespace kzn::vk
