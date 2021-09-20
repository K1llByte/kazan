#ifndef SIMPLE_RENDER_SYSTEM_H
#define SIMPLE_RENDER_SYSTEM_H

#include "pipeline.hpp"
#include "game_object.hpp"
#include "camera.hpp"

namespace kzn
{

class SimpleRenderSystem
{
private:

    Device&                   _device;
    std::unique_ptr<Pipeline> _pipeline;
    VkPipelineLayout          _pipeline_layout;
    VkDescriptorSetLayout     _descriptor_set_layout = VK_NULL_HANDLE;

public:

    SimpleRenderSystem(Device& device, VkRenderPass renderPass);
    ~SimpleRenderSystem();

    void render_game_objects(VkCommandBuffer command_buffer,
        std::vector<GameObject>& game_objects,
        const Camera& camera);

private:

    void create_pipeline_layout();
    void create_pipeline(VkRenderPass renderPass);
};

}

#endif // SIMPLE_RENDER_SYSTEM_H
