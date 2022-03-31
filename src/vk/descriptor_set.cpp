#include "vk/descriptor_set.hpp"

#include "vk/error.hpp"
#include "vk/utils.hpp"
#include "vk/cmd_buffers.hpp"

namespace kzn::vk
{
    DescriptorSetLayout::~DescriptorSetLayout()
    {
        Log::debug("DescriptorSetLayout destroyed");
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
        Log::debug("DescriptorSetLayout created");
        return set_layout;
    }

    DescriptorPool::DescriptorPool(Device* device)
    {
        std::vector<VkDescriptorPoolSize> pool_sizes{
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2 },
        };

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
        pool_info.pPoolSizes = pool_sizes.data();
        pool_info.maxSets = 2;

        auto result = vkCreateDescriptorPool(device->vk_device(), &pool_info, nullptr, &vkdescriptor_pool);
        VK_CHECK_MSG(result, "Failed to create descriptor pool!");
    }

    DescriptorPool::~DescriptorPool()
    {
        // Destroy Descriptor Pool
        vkDestroyDescriptorPool(device->vk_device(), vkdescriptor_pool, nullptr);        
    }

    DescriptorSet DescriptorPool::allocate(DescriptorSetLayout& descriptor_set_layout)
    {
        auto vk_descriptor_set_layout = descriptor_set_layout.vk_descriptor_set_layout();
        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = vkdescriptor_pool;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &vk_descriptor_set_layout;

        DescriptorSet descriptor_set;
        auto result = vkAllocateDescriptorSets(device->vk_device(), &alloc_info, &descriptor_set.vkdescriptor_set);
        VK_CHECK_MSG(result, "Failed to allocate descriptor sets!");

        // // Write to descriptor set
        // std::vector<VkWriteDescriptorSet> descriptor_writes
        // descriptor_writes.resize(buffers_count /* + samplers_count */);
        // for(std::size_t i = 0; i < buffers_count; ++i)
        // {
        //     descriptor_writes[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        //     descriptor_writes[j].dstSet = descriptor_set.vkdescriptor_set;
        //     descriptor_writes[j].dstBinding = i;
        //     descriptor_writes[j].dstArrayElement = 0;

        //     descriptor_writes[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        //     descriptor_writes[j].descriptorCount = 1;
        
        //     descriptor_writes[j].pBufferInfo = &descriptor_buffer_infos[j*image_count + i];
        //     descriptor_writes[j].pImageInfo = nullptr; // Optional
        //     descriptor_writes[j].pTexelBufferView = nullptr; // Optional
        // }

        // for(size_t j = 0; j < samplers_count; ++j)
        // {
        //     descriptor_writes[buffers_count + j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        //     descriptor_writes[buffers_count + j].dstSet = set.descriptor_sets[i];
        //     // FIXME: Might be wrong
        //     descriptor_writes[buffers_count + j].dstBinding = buffers_count + j;
        //     descriptor_writes[buffers_count + j].dstArrayElement = 0;

        //     descriptor_writes[buffers_count + j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        //     descriptor_writes[buffers_count + j].descriptorCount = 1;
        
        //     descriptor_writes[buffers_count + j].pBufferInfo = nullptr; // Optional
        //     descriptor_writes[buffers_count + j].pImageInfo = &descriptor_image_infos[j*image_count + i]; // Optional
        //     descriptor_writes[buffers_count + j].pTexelBufferView = nullptr; // Optional
        // }

        // vkUpdateDescriptorSets(device->device(), descriptor_writes.size(), descriptor_writes.data(), 0, nullptr);
        // descriptor_writes.clear();
        
        return descriptor_set;
    }

    void DescriptorSet::bind(CommandBuffer& cmd_buffer, VkPipelineLayout vkpipeline_layout)
    {
        vkCmdBindDescriptorSets(
            cmd_buffer.vk_command_buffer(),
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            vkpipeline_layout,
            0,
            1,
            &vkdescriptor_set,
            0,
            nullptr);
    }
} // namespace kzn::vk