#ifndef SIMPLE_RENDER_SYSTEM_H
#define SIMPLE_RENDER_SYSTEM_H

#include "renderer.hpp"
#include "pipeline.hpp"
#include "game_object.hpp"
#include "camera.hpp"

namespace kzn
{

struct PVM
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};


class SimpleRenderSystem
{
private:

    Device&                   _device;
    Renderer&                 _renderer;
    std::unique_ptr<Pipeline> _pipeline;
    VkPipelineLayout          _pipeline_layout;
    VkDescriptorSetLayout     _descriptor_set_layout = VK_NULL_HANDLE;

    UniformBuffer<PVM> _pvm_buffer;
    DescriptorSet      _descriptor_set;

public:

    SimpleRenderSystem(Device& device, Renderer& renderer);
    ~SimpleRenderSystem();

    void render_game_objects(
        VkCommandBuffer command_buffer,
        std::vector<GameObject>& game_objects,
        const Camera& camera);

private:

    void create_pipeline_layout();
    void create_pipeline(VkRenderPass renderPass);
};

}

#endif // SIMPLE_RENDER_SYSTEM_H
