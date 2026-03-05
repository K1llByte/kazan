#pragma once

#include "core/log.hpp"
#include <span>
#include <vector>
#include <unordered_map>

#include <vulkan/vulkan_core.h>

namespace kzn::vk {

using DescriptorSetBindings = std::vector<VkDescriptorSetLayoutBinding>;

VkDescriptorSetLayoutBinding uniform_binding(
    uint32_t binding,
    VkShaderStageFlags stage_flags = VK_SHADER_STAGE_ALL_GRAPHICS
);
VkDescriptorSetLayoutBinding sampler_binding(
    uint32_t binding,
    VkShaderStageFlags stage_flags = VK_SHADER_STAGE_ALL_GRAPHICS
);


struct DescriptorSetLayout {
    DescriptorSetBindings bindings;
    VkDescriptorSetLayout vk_layout;
};

class DescriptorSetLayoutCache {
public:
    struct DescriptorLayoutInfo {
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        bool operator==(const DescriptorLayoutInfo& other) const;

        size_t hash() const;
    };

    // Ctor
    DescriptorSetLayoutCache(VkDevice device);
    // Copy
    DescriptorSetLayoutCache(const DescriptorSetLayoutCache&) = delete;
    DescriptorSetLayoutCache& operator=(const DescriptorSetLayoutCache&) =
        delete;
    // Move
    DescriptorSetLayoutCache(DescriptorSetLayoutCache&&) = delete;
    DescriptorSetLayoutCache& operator=(DescriptorSetLayoutCache&&) = delete;
    // Dtor
    ~DescriptorSetLayoutCache();

    DescriptorSetLayout layout(const DescriptorSetBindings& info);

private:
    struct DescriptorLayoutHash {
        size_t operator()(const DescriptorLayoutInfo& info) const {
            return info.hash();
        }
    };

private:
    using Key = DescriptorLayoutInfo;
    using Value = VkDescriptorSetLayout;
    using DescriptorSetLayoutMap =
        std::unordered_map<Key, Value, DescriptorLayoutHash>;
    VkDevice m_vk_device;
    DescriptorSetLayoutMap m_layout_cache;
};

} // namespace kzn::vk