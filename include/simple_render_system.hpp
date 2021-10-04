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

struct CameraData
{
    glm::vec3 position;
};

struct PVMData
{
    glm::mat4 proj_view;
    glm::mat4 model;
};


class SimpleRenderSystem
{
private:

    Device&                   _device;
    Renderer&                 _renderer;
    std::unique_ptr<Pipeline> _pipeline;
    VkPipelineLayout          _pipeline_layout;
    VkDescriptorSetLayout     _descriptor_set_layout = VK_NULL_HANDLE;

    DescriptorPool            _pool;
    PushConstant<PVMData>     _pvm_push;
    UniformBuffer<CameraData> _cam_buffer;
    DescriptorSet             _set1;

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
