#include "simple_render_system.hpp"

#include <glm/glm.hpp>
#include <stdexcept>

// Debug
#include <iostream>

namespace kzn
{


SimpleRenderSystem::SimpleRenderSystem(Device& device, Renderer& renderer)
    : _device{device},
    _renderer{renderer}
{
    create_pipeline_layout();
    create_pipeline(renderer.render_pass());
    
    // lights.lights[0] = PointLight(
    //     glm::vec3{2.0f, 2.0f, 2.0f},
    //     0.2f,
    //     1.f,
    //     0.0f);
    lights.lights[0] = DirLight(
        glm::normalize(glm::vec3{1.0, -3.0, -1.0}));
    lights.used = 1;
}


SimpleRenderSystem::~SimpleRenderSystem()
{
    _cam_buffer.cleanup();
    _lights_buffer.cleanup();
    _pool.cleanup();
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

    _cam_buffer.update({
        .position = camera.position()
    });
    _lights_buffer.update(lights);
    // glm::mat4 projection_view = camera.projection() * camera.view();

    // std::cout << "render loop (camera update above)\n";
    _pipeline->bind(command_buffer);
    PVMData push_data{};
    push_data.proj_view = camera.projection() * camera.view();
    
    for(auto& obj : game_objects)
    {
        // obj.transform.translation.y = glm::mod(obj.transform.translation.y + 0.01f, glm::two_pi<float>());
        // obj.transform.translation.x = glm::mod(obj.transform.translation.x + 0.005f, 1.f);

        push_data.model = obj.transform.mat4();

        _pvm_push.push(command_buffer, _pipeline_layout, push_data);

        _set1.bind(command_buffer, _pipeline_layout);

        obj.model->bind(command_buffer);
        obj.model->draw(command_buffer);
    }
}


void SimpleRenderSystem::create_pipeline_layout()
{
    // auto dsb = descriptor_pool.descriptor_set_builder(); //DescriptorSetBuilder();
    // UniformBuffer<PVM> pvm_ub = dsb.create_uniform_buffer(VK_SHADER_STAGE_VERTEX_BIT);
    // DescriptorSet descriptor_set = dsb.build();

    
    // PushConstant<PVM> pvm_pc(VK_SHADER_STAGE_VERTEX_BIT);
    // VkPipelineLayout layout = PipelineLayoutBuilder()
    //     .push_constant(pvm_pc)
    //     .descriptor_set(descriptor_set)
    //     .build();

    ///////////////// Push Constants //////////////////

    // VkPushConstantRange push_constant_range{};
    // push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    // push_constant_range.offset = 0;
    // push_constant_range.size = sizeof(PushConstantsData);

    // ///////////////// Uniform Buffers /////////////////

    // std::vector<VkDescriptorSetLayoutBinding> layout_bindings(1);

    // // _pvm_buffer = _renderer.alloc_buffer<PVM>(&layout_bindings[0]);
    // _cam_buffer = _renderer.alloc_buffer<CameraData>(&layout_bindings[0]);

    // VkDescriptorSetLayoutCreateInfo layout_info{};
    // layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    // layout_info.bindingCount = static_cast<uint32_t>(layout_bindings.size());
    // layout_info.pBindings = layout_bindings.data();

    // if(vkCreateDescriptorSetLayout(_device.device(), &layout_info, nullptr, &_descriptor_set_layout) != VK_SUCCESS)
    // {
    //     throw std::runtime_error("failed to create descriptor set layout!");
    // }

    // _renderer.init_descriptor_pool(_descriptor_set_layout);
    // _descriptor_set = _renderer.init_descriptor_set();

    // ///////////////////////////////////////////////////

    // VkPipelineLayoutCreateInfo pipeline_layout_info{};
    // pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    // // // With Push Constants
    // // pipeline_layout_info.setLayoutCount = 0;
    // // pipeline_layout_info.pSetLayouts = nullptr;
    // // pipeline_layout_info.pushConstantRangeCount = 1;
    // // pipeline_layout_info.pPushConstantRanges = &push_constant_range;

    // // // With Uniform Buffers
    // // pipeline_layout_info.setLayoutCount = 1;
    // // pipeline_layout_info.pSetLayouts = &_descriptor_set_layout;
    // // pipeline_layout_info.pushConstantRangeCount = 0;
    // // pipeline_layout_info.pPushConstantRanges = nullptr;

    // // With Both
    // pipeline_layout_info.setLayoutCount = 1;
    // pipeline_layout_info.pSetLayouts = &_descriptor_set_layout;
    // pipeline_layout_info.pushConstantRangeCount = 1;
    // pipeline_layout_info.pPushConstantRanges = &push_constant_range;

    // if(vkCreatePipelineLayout(_device.device(), &pipeline_layout_info, nullptr, &_pipeline_layout) != VK_SUCCESS)
    // {
    //     throw std::runtime_error("failed to create pipeline layout!");
    // }

    ///////////////// Refactored code /////////////////

    _pool = _renderer.create_descriptor_pool({});

    DescriptorSetBuilder ds_builder = _pool.descriptor_set_builder();
    _cam_buffer = ds_builder.create_uniform_buffer<CameraData>();
    _lights_buffer = ds_builder.create_uniform_buffer<LightsData>(VK_SHADER_STAGE_FRAGMENT_BIT);
    _set1 = ds_builder.build();

    _pvm_push = PushConstant<PVMData>(
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

    PipelineLayoutBuilder layout_builder = _renderer.pipeline_layout_builder();
    _pipeline_layout = layout_builder
        .add_descriptor_set(_set1)
        .add_push_constant(_pvm_push)
        .build();
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

    // TODO: Try to change unique_ptr to
    // a stack allocated alternative
    _pipeline = std::make_unique<Pipeline>(
        _device,
        "shaders/surface/surface.vert.spv",
        "shaders/surface/surface.frag.spv",
        pipeline_config);
}

}