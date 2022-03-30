#include "vk/descriptor_set.hpp"

#include "vk/error.hpp"
#include "vk/utils.hpp"

namespace kzn::vk
{
    DescriptorSetLayout::~DescriptorSetLayout()
    {
        Log::warning("DescriptorSetLayout destroyed");
        vkDestroyDescriptorSetLayout(device->vk_device(), vkset_layout, nullptr);
    }

    DescriptorSetLayoutBuilder::DescriptorSetLayoutBuilder(Device* _device)
        : device(_device) {}

    DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::add_binding(
        const uint32_t binding,
        const VkDescriptorType desc_type,
        const uint32_t count,
        const VkShaderStageFlags stages)
    {
        bindings.push_back(VkDescriptorSetLayoutBinding{
            .binding = binding,
            .descriptorType = desc_type,
            .descriptorCount = count,
            .stageFlags = stages,
            .pImmutableSamplers = nullptr,
        });
        return *this;
    }

    DescriptorSetLayout DescriptorSetLayoutBuilder::build()
    {
        VkDescriptorSetLayoutCreateInfo layout_info{};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
        layout_info.pBindings = bindings.data();

        // vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
        DescriptorSetLayout set_layout;
        auto result = vkCreateDescriptorSetLayout(device->vk_device(), &layout_info, nullptr, &set_layout.vkset_layout);
        VK_CHECK_MSG(result, "Failed to create descriptor set layout!");
        set_layout.device = device;
        Log::warning("DescriptorSetLayout created");
        return set_layout;
    }
} // namespace kzn::vk