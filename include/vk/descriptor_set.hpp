#ifndef KZN_VK_DESCRIPTOR_SET_HPP
#define KZN_VK_DESCRIPTOR_SET_HPP

#include "vk/device.hpp"

namespace kzn::vk
{
    class DescriptorSetLayout
    {
        friend class DescriptorSetLayoutBuilder;
    public:
        ~DescriptorSetLayout();

        VkDescriptorSetLayout vk_descriptor_set_layout() { return vkset_layout; };

    private:
        DescriptorSetLayout() = default;
    private:
        Device*               device;
        VkDescriptorSetLayout vkset_layout;
    };

    class DescriptorSetLayoutBuilder
    {
    public:
        DescriptorSetLayoutBuilder(Device* _device);
        ~DescriptorSetLayoutBuilder() = default;

        DescriptorSetLayoutBuilder& add_binding(
            const uint32_t binding,
            const VkDescriptorType desc_type,
            const uint32_t count,
            const VkShaderStageFlags stages);

        DescriptorSetLayout build();

    private:
        Device*                                   device;
        std::vector<VkDescriptorSetLayoutBinding> bindings;
    };

    class DescriptorSet
    {
        friend class DescriptorPool;
    public:
        ~DescriptorSet() = default;

        VkDescriptorSet vk_descriptor_set() noexcept { return vkdescriptor_set; }

        void bind(CommandBuffer& cmd_buffer, VkPipelineLayout vkpipeline_layout);

    private:
        DescriptorSet() = default;

    private:
        VkDescriptorSet vkdescriptor_set;
    };

    class DescriptorPool
    {
    public:
        DescriptorPool(Device* device);
        ~DescriptorPool();

        VkDescriptorPool vk_descriptor_pool() noexcept { return vkdescriptor_pool; }

        DescriptorSet allocate(DescriptorSetLayout& descriptor_set_layout);
        // TODO: allocate multiple sets at a time

    private:
        Device*          device;
        VkDescriptorPool vkdescriptor_pool;
    };
} // namespace kzn::vk

#endif // KZN_VK_DESCRIPTOR_SET_HPP