#ifndef MULTIPLE_RENDER_SYSTEM_H
#define MULTIPLE_RENDER_SYSTEM_H

#include "renderer.hpp"
#include "pipeline.hpp"
#include "game_object.hpp"
#include "camera.hpp"
#include "light.hpp"

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

struct LightsData
{
    alignas(4) uint  used = 0;
    alignas(16) Light lights[8];
};


class MultipleRenderSystem
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
    UniformBuffer<LightsData> _lights_buffer;
    DescriptorSet             _set1;

    LightsData lights;

public:

    MultipleRenderSystem(Device& device, Renderer& renderer);
    ~MultipleRenderSystem();

    void render_game_objects(
        VkCommandBuffer command_buffer,
        std::vector<GameObject>& game_objects,
        const Camera& camera);

private:

    void create_pipeline_layout();
    void create_pipeline(VkRenderPass renderPass);
};

}

#endif // MULTIPLE_RENDER_SYSTEM_H