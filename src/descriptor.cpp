#include "descriptor.hpp"

namespace kzn
{

void DescriptorSet::bind(VkCommandBuffer command_buffer, VkPipelineLayout layout) const
{
    vkCmdBindDescriptorSets(
        command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        layout,
        0,
        1,
        &descriptor_sets[*current_index],
        0,
        nullptr);
}


DescriptorSet DescriptorSetBuilder::build()
{
    // Create VkDescriptorSetLayout
    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = static_cast<uint32_t>(layout_bindings.size());
    layout_info.pBindings = layout_bindings.data();

    VkDescriptorSetLayout descriptor_set_layout;
    if(vkCreateDescriptorSetLayout(device->device(), &layout_info, nullptr, &descriptor_set_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    // Allocate DescriptorSets
    std::vector<VkDescriptorSetLayout> layouts(image_count, descriptor_set_layout);
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pool;
    alloc_info.descriptorSetCount = layouts.size();
    alloc_info.pSetLayouts = layouts.data();

    auto set = DescriptorSet{
        .current_index = current_index,
        .descriptor_sets = std::vector<VkDescriptorSet>(image_count)
    };
    
    if(vkAllocateDescriptorSets(device->device(), &alloc_info, set.descriptor_sets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    // Update DescriptorSets
    const size_t buffers_count = descriptor_buffer_infos.size() / image_count;

    std::vector<VkWriteDescriptorSet> descriptor_writes(buffers_count);

    for(size_t i = 0; i < image_count; ++i)
    {
        // descriptor_writes.resize(buffers_count);
        for(size_t j = 0; j < buffers_count; ++j)
        {
            descriptor_writes[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_writes[j].dstSet = set.descriptor_sets[i];
            descriptor_writes[j].dstBinding = j;
            descriptor_writes[j].dstArrayElement = 0;

            descriptor_writes[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptor_writes[j].descriptorCount = 1;
        
            descriptor_writes[j].pBufferInfo = &descriptor_buffer_infos[j*image_count + i];
            descriptor_writes[j].pImageInfo = nullptr; // Optional
            descriptor_writes[j].pTexelBufferView = nullptr; // Optional
        }

        vkUpdateDescriptorSets(device->device(), descriptor_writes.size(), descriptor_writes.data(), 0, nullptr);
        descriptor_writes.clear();
    }

    return set;
}


DescriptorSetBuilder DescriptorPool::descriptor_set_builder()
{
    return DescriptorSetBuilder{
        .device = this->device,
        .pool = this->pool,
        .current_index = this->current_index,
        .image_count = this->image_count
    };
}

}
