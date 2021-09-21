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


SimpleRenderSystem::SimpleRenderSystem(Device& device, Renderer& renderer)
    : _device{device},
    _renderer{renderer}
{
    create_pipeline_layout();
    create_pipeline(renderer.render_pass());
}


SimpleRenderSystem::~SimpleRenderSystem()
{
    if(_descriptor_set_layout != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(_device.device(), _descriptor_set_layout, nullptr);
    }
    vkDestroyPipelineLayout(_device.device(), _pipeline_layout, nullptr);
}


void SimpleRenderSystem::render_game_objects(
    VkCommandBuffer command_buffer,
    std::vector<GameObject>& game_objects,
    const Camera& camera)
{

    _pipeline->bind(command_buffer);
    // auto projection_view = camera.projection() * camera.view();
    
    for(auto& obj : game_objects)
    {
        // obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.01f, glm::two_pi<float>());
        // obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.005f, glm::two_pi<float>());

        // PushConstantsData push_data{};
        // push_data.pvm = projection_view * obj.transform.mat4();

        // vkCmdPushConstants(
        //     command_buffer,
        //     _pipeline_layout,
        //     VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        //     0,
        //     sizeof(PushConstantsData),
        //     &push_data);

        _pvm_buffer.update({
            .model = obj.transform.mat4(),
            .view = camera.view(),
            .projection = camera.projection(),
        });
        _descriptor_set = _renderer.init_descriptor_set();

        obj.model->bind(command_buffer);
        _descriptor_set.bind(command_buffer, _pipeline_layout);
        obj.model->draw(command_buffer);
    }
}


void SimpleRenderSystem::create_pipeline_layout()
{
    ///////////////// Push Constants //////////////////

    // VkPushConstantRange push_constant_range{};
    // push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    // push_constant_range.offset = 0;
    // push_constant_range.size = sizeof(PushConstantsData);

    ///////////////// Uniform Buffers /////////////////

    // VkDescriptorSetLayoutBinding ubo_layout_binding{};
    // ubo_layout_binding.binding = 0;
    // ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // ubo_layout_binding.descriptorCount = 1;
    // ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    // ubo_layout_binding.pImmutableSamplers = nullptr; // Optional
    
    // VkDescriptorSetLayoutCreateInfo layout_info{};
    // layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    // layout_info.bindingCount = 1;
    // layout_info.pBindings = &ubo_layout_binding;

    // if (vkCreateDescriptorSetLayout(_device.device(), &layout_info, nullptr, &_descriptor_set_layout) != VK_SUCCESS)
    // {
    //     throw std::runtime_error("failed to create descriptor set layout!");
    // }

    std::vector<VkDescriptorSetLayoutBinding> layout_bindings(1);

    std::cout << "debug 1\n";
    _pvm_buffer = _renderer.alloc_buffer<PVM>(&layout_bindings[0]);
    std::cout << "debug 2\n";
    // _pvm_buffer = _renderer.alloc_buffer<PVM>(&layout_bindings[0]);
    // buffer2 = _renderer.alloc_buffer<PushConstantsData>(&layout_bindings[1]);

    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = layout_bindings.size();
    layout_info.pBindings = layout_bindings.data();

    if(vkCreateDescriptorSetLayout(_device.device(), &layout_info, nullptr, &_descriptor_set_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    _renderer.init_descriptor_pool(_descriptor_set_layout);
    _descriptor_set = _renderer.init_descriptor_set();

    ///////////////////////////////////////////////////

    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    
    // // With Push Constants
    // pipeline_layout_info.setLayoutCount = 0;
    // pipeline_layout_info.pSetLayouts = nullptr;
    // pipeline_layout_info.pushConstantRangeCount = 1;
    // pipeline_layout_info.pPushConstantRanges = &push_constant_range;

    // With Uniform Buffers
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &_descriptor_set_layout;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;

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
  
    PipelineConfig pipeline_config = 
        PipelineConfigBuilder(_pipeline_layout, render_pass)
        .set_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .build();
    // PipelineConfig pipeline_config{};
    // Pipeline::default_pipeline_config_info(pipeline_config);
    // pipeline_config.render_pass = render_pass;
    // pipeline_config.pipeline_layout = _pipeline_layout;

    // TODO: Try to change unique_ptr to
    // a stack allocated alternative
    _pipeline = std::make_unique<Pipeline>(
        _device,
        "shaders/mesh.vert.spv",
        "shaders/mesh.frag.spv",
        pipeline_config);
}

}
