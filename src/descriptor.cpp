#include "descriptor.hpp"

namespace kzn
{

void DescriptorSet::cleanup()
{
    if(descriptor_set_layout != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(device->device(), descriptor_set_layout, nullptr);
    }
}

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


void DescriptorSetBuilder::create_image_sampler(Texture* tex, VkShaderStageFlags stage)
{
    for(size_t i = 0; i < image_count; ++i)
    {
        descriptor_image_infos.push_back(
            VkDescriptorImageInfo {
                .sampler = tex->sampler(),
                .imageView = tex->image_view(),
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            }
        );
    }

    // TODO: change 'static_cast<uint32_t>(descriptor_buffer_infos.size()) / image_count'
    // to 'next_binding()' aux private function
    layout_bindings.push_back({
        .binding =            static_cast<uint32_t>(descriptor_buffer_infos.size()) / image_count,
        .descriptorType =     VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount =    1,
        .stageFlags =         stage,
        .pImmutableSamplers = nullptr
    });
}


DescriptorSet DescriptorSetBuilder::build()
{
    // Create VkDescriptorSetLayout
    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = static_cast<uint32_t>(layout_bindings.size());
    layout_info.pBindings = layout_bindings.data();

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
        .device = device,
        .current_index = current_index,
        .descriptor_sets = std::vector<VkDescriptorSet>(image_count),
        .descriptor_set_layout = descriptor_set_layout
    };
    
    if(vkAllocateDescriptorSets(device->device(), &alloc_info, set.descriptor_sets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    // Update DescriptorSets
    const size_t buffers_count = descriptor_buffer_infos.size() / image_count;
    const size_t samplers_count = descriptor_image_infos.size() / image_count;

    std::vector<VkWriteDescriptorSet> descriptor_writes;

    for(size_t i = 0; i < image_count; ++i)
    {
        descriptor_writes.resize(buffers_count + samplers_count);
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

        for(size_t j = 0; j < samplers_count; ++j)
        {
            descriptor_writes[buffers_count + j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_writes[buffers_count + j].dstSet = set.descriptor_sets[i];
            // FIXME: Might be wrong
            descriptor_writes[buffers_count + j].dstBinding = buffers_count + j;
            descriptor_writes[buffers_count + j].dstArrayElement = 0;

            descriptor_writes[buffers_count + j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptor_writes[buffers_count + j].descriptorCount = 1;
        
            descriptor_writes[buffers_count + j].pBufferInfo = nullptr; // Optional
            descriptor_writes[buffers_count + j].pImageInfo = &descriptor_image_infos[j*image_count + i]; // Optional
            descriptor_writes[buffers_count + j].pTexelBufferView = nullptr; // Optional
        }

        vkUpdateDescriptorSets(device->device(), descriptor_writes.size(), descriptor_writes.data(), 0, nullptr);
        descriptor_writes.clear();
    }

    return set;
}


void DescriptorPool::cleanup()
{
    if(pool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(device->device(), pool, nullptr);
    }
}


DescriptorSetBuilder DescriptorPool::descriptor_set_builder()
{
    return DescriptorSetBuilder{
        .device = this->device,
        .pool = this->pool,
        .current_index = this->current_index,
        .image_count = this->image_count,
    };
}

}
