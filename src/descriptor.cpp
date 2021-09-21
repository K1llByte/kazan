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

}
