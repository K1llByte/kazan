#include "simple_render_system.hpp"

#include <glm/glm.hpp>
#include <stdexcept>

// Debug
#include <iostream>

namespace kzn
{

struct PushConstantsData
{
    glm::vec4 data;
    glm::mat4 pvm;
};


SimpleRenderSystem::SimpleRenderSystem(Device& device, VkRenderPass render_pass)
    : _device{device}
{
    create_pipeline_layout();
    create_pipeline(render_pass);
}


SimpleRenderSystem::~SimpleRenderSystem()
{
    vkDestroyPipelineLayout(_device.device(), _pipeline_layout, nullptr);
}


void SimpleRenderSystem::render_game_objects(
    VkCommandBuffer command_buffer,
    std::vector<GameObject>& game_objects,
    const Camera& camera)
{
    _pipeline->bind(command_buffer);

    auto projection_view = camera.projection() * camera.view();

    for(auto& obj : game_objects)
    {
        // obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.01f, glm::two_pi<float>());
        // obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.005f, glm::two_pi<float>());

        PushConstantsData push_data{};
        push_data.pvm = projection_view * obj.transform.mat4();

        vkCmdPushConstants(
            command_buffer,
            _pipeline_layout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(PushConstantsData),
            &push_data);
        obj.model->bind(command_buffer);
        obj.model->draw(command_buffer);
    }
}


void SimpleRenderSystem::create_pipeline_layout()
{
    VkPushConstantRange push_constant_range{};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(PushConstantsData);

    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pSetLayouts = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &push_constant_range;
    if(vkCreatePipelineLayout(_device.device(), &pipeline_layout_info, nullptr, &_pipeline_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}


void SimpleRenderSystem::create_pipeline(VkRenderPass render_pass)
{
    if(_pipeline_layout == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Cannot create pipeline before pipeline layout");
    }
  
    PipelineConfig pipeline_config{};
    Pipeline::default_pipeline_config_info(pipeline_config);
    pipeline_config.render_pass = render_pass;
    pipeline_config.pipeline_layout = _pipeline_layout;
    // TODO: Try to change unique_ptr to
    // a stack allocated alternative
    _pipeline = std::make_unique<Pipeline>(
        _device,
        "shaders/mesh.vert.spv",
        "shaders/mesh.frag.spv",
        pipeline_config);
}

}
