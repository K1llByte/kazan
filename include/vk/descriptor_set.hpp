#ifndef KZN_VK_DESCRIPTOR_SET_HPP
#define KZN_VK_DESCRIPTOR_SET_HPP

#include "vk/device.hpp"
#include "vk/cmd_buffers.hpp"
#include "core/log.hpp"

#include <initializer_list>
#include <unordered_map>

namespace kzn::vk {
    struct BufferBinding {
        uint32_t               binding;
        VkDescriptorBufferInfo info;
        VkDescriptorType       type;
        VkShaderStageFlags     stages = VK_SHADER_STAGE_ALL_GRAPHICS;

        constexpr BufferBinding(
            uint32_t               _binding,
            VkDescriptorBufferInfo _info,
            VkDescriptorType       _type,
            VkShaderStageFlags     _stages = VK_SHADER_STAGE_ALL_GRAPHICS)
            : binding(_binding)
            , info(_info)
            , type(_type)
            , stages(_stages) {}

        static constexpr BufferBinding uniform(
            uint32_t               _binding,
            VkDescriptorBufferInfo _info,
            VkShaderStageFlags     _stages = VK_SHADER_STAGE_ALL_GRAPHICS)
        {
            return BufferBinding{_binding, _info, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, _stages};
        }
    };

    // TODO: Do this later
    // struct MultipleBufferBinding {
    //     uint32_t                            binding;
    //     std::vector<VkDescriptorBufferInfo> info;
    //     VkDescriptorType                    type;
    //     VkShaderStageFlags                  stages = VK_SHADER_STAGE_ALL_GRAPHICS;

    //     constexpr MultipleBufferBinding(
    //         uint32_t               _binding,
    //         std::initializer_list<VkDescriptorBufferInfo> _infos,
    //         VkDescriptorType       _type,
    //         VkShaderStageFlags     _stages = VK_SHADER_STAGE_ALL_GRAPHICS)
    //         : binding(_binding)
    //         , infos(_infos)
    //         , type(_type)
    //         , stages(_stages) {}

    //     static constexpr BufferBinding uniform(
    //         uint32_t                                      _binding,
    //         std::initializer_list<VkDescriptorBufferInfo> _infos,
    //         VkShaderStageFlags                            _stages = VK_SHADER_STAGE_ALL_GRAPHICS)
    //     {
    //         return MultipleBufferBinding{_binding, _infos, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, _stages};
    //     }
    // };


    // DescriptorSetAllocator is a manager of DescriptorSetPool's which creates new pools
    // when its needed
    class DescriptorSetAllocator {
        public:
        using PoolSizes = std::vector<std::pair<VkDescriptorType,float>>;

        DescriptorSetAllocator(Device* _device);
        ~DescriptorSetAllocator();

        void reset_pools();
        VkDescriptorSet allocate(const VkDescriptorSetLayout& layout);


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

        VkDescriptorSetLayout create_descriptor_layout(
            VkDescriptorSetLayoutCreateInfo* info); // TODO:

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