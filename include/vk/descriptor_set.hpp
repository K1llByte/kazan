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
} // namespace kzn::vk

#endif // KZN_VK_DESCRIPTOR_SET_HPP