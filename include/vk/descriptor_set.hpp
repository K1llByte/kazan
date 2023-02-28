#ifndef KZN_VK_DESCRIPTOR_SET_HPP
#define KZN_VK_DESCRIPTOR_SET_HPP

#include "vk/device.hpp"
#include "vk/cmd_buffers.hpp"
#include "core/log.hpp"

#include <initializer_list>
#include <unordered_map>

namespace kzn::vk {
    class DescriptorSetLayout {
        private:
        std::vector<VkDescriptorSetLayoutBinding> m_layout_bindings;
    };
    
    class DescriptorSetLayoutBuilder {
        public:
        DescriptorSetLayoutBuilder() = default;
        ~DescriptorSetLayoutBuilder() = default;


        VkDescriptorSetLayout build(vk::Device& _device);

        private:
        std::vector<VkDescriptorSetLayoutBinding> m_layout_bindings;
    };


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

        VkDescriptorSetLayout create_layout(
            VkDescriptorSetLayoutCreateInfo* info);

        private:
        struct DescriptorLayoutHash {
            std::size_t operator()(const DescriptorLayoutInfo& info) const { 
                return info.hash();
            }
        };

        private:
        Device* device;
        std::unordered_map<
            DescriptorLayoutInfo, 
            VkDescriptorSetLayout,
            DescriptorLayoutHash> layout_cache;
    };


    class DescriptorSet {
        public:
        DescriptorSet(
            Device*                                         _device,
            DescriptorSetAllocator&                         _allocator,
            DescriptorSetLayoutCache&                       _cache,
            const std::initializer_list<vk::BufferBinding>& _bindings);
        
        // TODO: Do later
        // static std::vector<DescriptorSet> multiple(
        //     size_t                                          _num_sets,
        //     Device*                                         _device,
        //     DescriptorSetAllocator&                         _allocator,
        //     DescriptorSetLayoutCache&                       _cache,
        //     const std::initializer_list<vk::MultipleBufferBinding>& _bindings);

        VkDescriptorSetLayout layout() { return set_layout; }

        void bind(vk::CommandBuffer& cmd_buffer, VkPipelineLayout pipeline_layout) const;
        // TODO: Do later
        // static void bind_multiple(...) const;

        private:
        VkDescriptorSet        vk_descriptor_set;
        VkDescriptorSetLayout  set_layout = VK_NULL_HANDLE;
        Device*                device;
    };
} // namespace kzn::vk

#endif // KZN_VK_DESCRIPTOR_SET_HPP