#pragma once

#include <span>
#include <unordered_map>

#include "device.hpp"

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

class DescriptorSetLayoutCache;

class DescriptorSetLayout {
public:
    friend class DescriptorSetLayoutCache;

    // Copy
    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
    // Move
    DescriptorSetLayout(DescriptorSetLayout&&) = default;
    DescriptorSetLayout& operator=(DescriptorSetLayout&&) = default;
    // Dtor
    ~DescriptorSetLayout() = default;

    std::span<const VkDescriptorSetLayoutBinding> bindings() const {
        return m_layout_bindings;
    };

    operator VkDescriptorSetLayout() const { return m_layout; }

private:
    DescriptorSetBindings m_layout_bindings;
    VkDescriptorSetLayout m_layout;

private:
    // Ctor
    DescriptorSetLayout(
        DescriptorSetBindings&& layout_bindings,
        VkDescriptorSetLayout layout
    );
};

class DescriptorSetLayoutCache {
public:
    struct DescriptorLayoutInfo {
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        bool operator==(const DescriptorLayoutInfo& other) const;

        size_t hash() const;
    };

    // Ctor
    DescriptorSetLayoutCache(Device& device);
    // Copy
    DescriptorSetLayoutCache(const DescriptorSetLayoutCache&) = delete;
    DescriptorSetLayoutCache& operator=(const DescriptorSetLayoutCache&) =
        delete;
    // Move
    DescriptorSetLayoutCache(DescriptorSetLayoutCache&&) = delete;
    DescriptorSetLayoutCache& operator=(DescriptorSetLayoutCache&&) = delete;
    // Dtor
    ~DescriptorSetLayoutCache();

    DescriptorSetLayout create_layout(const DescriptorSetBindings& info);

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
    Device& m_device;
    DescriptorSetLayoutMap m_layout_cache;
};

} // namespace kzn::vk