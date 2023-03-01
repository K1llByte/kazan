#ifndef KZN_VK_DESCRIPTOR_SET_HPP
#define KZN_VK_DESCRIPTOR_SET_HPP

#include "vk/device.hpp"
#include "vk/cmd_buffers.hpp"
#include "core/log.hpp"

#include <initializer_list>
#include <unordered_map>
#include <span>

namespace kzn::vk {
    // DescriptorSetAllocator is a manager of DescriptorSetPool's which creates new pools
    // when its needed
    class DescriptorSetAllocator {
        public:
        using PoolSizes = std::vector<std::pair<VkDescriptorType,float>>;

        DescriptorSetAllocator(Device* _device);
        ~DescriptorSetAllocator();

        void reset_pools();
        VkDescriptorSet allocate(const VkDescriptorSetLayout& layout);
        // TODO: // DescriptorSet allocate(const DescriptorSetLayout& layout);

        private:
        Device*                       device;
        VkDescriptorPool              current_pool = VK_NULL_HANDLE;
        PoolSizes                     descriptor_sizes;
        std::vector<VkDescriptorPool> used_pools;
        std::vector<VkDescriptorPool> free_pools;

        private:
        VkDescriptorPool grab_pool();
    };


    class DescriptorSetLayout;

    class DescriptorSetLayoutCache {
        public:
        struct DescriptorLayoutInfo {
            // TODO: Turn this into an inlined array
            std::vector<VkDescriptorSetLayoutBinding> bindings;

            bool operator==(const DescriptorLayoutInfo& other) const;

            size_t hash() const;
        };
        
        DescriptorSetLayoutCache(Device* _device);
        ~DescriptorSetLayoutCache();

        DescriptorSetLayout create_layout(
            std::vector<VkDescriptorSetLayoutBinding>&& info);

        private:
        struct DescriptorLayoutHash {
            std::size_t operator()(const DescriptorLayoutInfo& info) const { 
                return info.hash();
            }
        };

        private:
        using Key = DescriptorLayoutInfo;
        using Value = VkDescriptorSetLayout;
        Device* device;
        std::unordered_map<
            Key,
            Value,
            DescriptorLayoutHash> layout_cache;
    };


    class DescriptorSetLayout {
        friend class DescriptorSetLayoutCache;

        public:
        std::span<VkDescriptorSetLayoutBinding> bindings() const;
        VkDescriptorSetLayout vk_layout() const { return m_layout; }

        private:
        std::span<VkDescriptorSetLayoutBinding> m_layout_bindings;
        VkDescriptorSetLayout                   m_layout;

        private:
        DescriptorSetLayout(
            std::span<VkDescriptorSetLayoutBinding> layout_bindings,
            VkDescriptorSetLayout                   layout)
            : m_layout_bindings(layout_bindings)
            , m_layout(layout)
        {

        }
    };


    class DescriptorSetLayoutBuilder {
        public:
        DescriptorSetLayoutBuilder() = default;
        ~DescriptorSetLayoutBuilder() = default;

        DescriptorSetLayoutBuilder& add_uniform(
            uint32_t binding,
            VkShaderStageFlags stage_flags = VK_SHADER_STAGE_ALL_GRAPHICS);

        DescriptorSetLayout build(
            DescriptorSetLayoutCache& _cache);

        private:
        std::vector<VkDescriptorSetLayoutBinding> m_layout_bindings;
    };


    class DescriptorSet {
        public:
        
        DescriptorSetLayout layout() { return m_set_layout; }

        void bind(vk::CommandBuffer& cmd_buffer, VkPipelineLayout pipeline_layout) const;

        private:
        Device*             m_device;
        VkDescriptorSet     m_vk_descriptor_set;
        DescriptorSetLayout m_set_layout;

        private:
        DescriptorSet(
            Device*             device,
            VkDescriptorSet     descriptor_set,
            DescriptorSetLayout layout);
    };
} // namespace kzn::vk

#endif // KZN_VK_DESCRIPTOR_SET_HPP