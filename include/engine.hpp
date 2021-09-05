#ifndef ENGINE_H
#define ENGINE_H

#include "types.hpp"
#include "mesh.hpp"

#include <vector>
#include <functional>
#include <deque>

#include <glm/glm.hpp>

namespace kzn
{

struct PipelineBuilder
{
    std::vector<VkPipelineShaderStageCreateInfo> _shader_stages;
    VkPipelineVertexInputStateCreateInfo         _vertex_input_info;
    VkPipelineInputAssemblyStateCreateInfo       _input_assembly;
    VkViewport                                   _viewport;
    VkRect2D                                     _scissor;
    VkPipelineRasterizationStateCreateInfo       _rasterizer;
    VkPipelineColorBlendAttachmentState          _color_blend_attachment;
    VkPipelineMultisampleStateCreateInfo         _multisampling;
    VkPipelineLayout                             _pipeline_layout;
    VkPipelineDepthStencilStateCreateInfo        _depth_stencil;

    VkPipeline build(VkDevice device, VkRenderPass pass);
};


struct MeshPushConstants
{
    glm::vec4 data;
    glm::mat4 pvm;
};


struct DeletionQueue
{
    std::deque<std::function<void()>> deletors;

    void push_function(std::function<void()>&& function)
    {
        deletors.push_back(function);
    }

    void flush()
    {
        // Reverse iterate the deletion queue to execute all the functions
        for (auto it = deletors.rbegin(); it != deletors.rend(); ++it)
        {
            (*it)();
        }

        deletors.clear();
    }
};


class Engine
{
public:

    bool _is_initialized{ false };
    int  _frame_number{ 0 };

    VkExtent2D _window_extent{ 1700 , 900 };

    struct GLFWwindow* _window{ nullptr };

    VkInstance               _instance; // Vulkan library handle
    VkDebugUtilsMessengerEXT _debug_messenger; // Vulkan debug output handle
    VkPhysicalDevice         _physical_device; // GPU chosen as the default device
    VkDevice                 _device; // Vulkan device for commands
    VkSurfaceKHR             _surface; // Vulkan window surface

    VkSwapchainKHR           _swapchain;
    VkFormat                 _swapchain_image_format;
    std::vector<VkImage>     _swapchain_images;
    std::vector<VkImageView> _swapchain_image_views;

    VkQueue  _graphics_queue;
    uint32_t _graphics_queue_family;

    VkCommandPool   _command_pool;
    VkCommandBuffer _main_command_buffer;

    VkRenderPass               _render_pass;
    std::vector<VkFramebuffer> _framebuffers;

    VkSemaphore _present_semaphore;
    VkSemaphore _render_semaphore;
    VkFence     _render_fence;

    VkPipelineLayout _triangle_pipeline_layout;
    VkPipeline       _triangle_pipeline;

    DeletionQueue _main_deletion_queue;

    VmaAllocator _allocator; // VMA lib allocator

    VkPipeline _mesh_pipeline;
    Mesh _triangle_mesh;

    VkImageView    _depth_image_view;
    AllocatedImage _depth_image;
    VkFormat       _depth_format;

public:

    // Initializes necessary objects
    void init();

    // Destroys every objects
    void cleanup();

    // Draw loop
    void draw();

    // Run main loop
    void run();

    //loads a shader module from a spir-v file. Returns false if it errors
    bool load_shader_module(const char* file_path, VkShaderModule* out_shader_module);

private:

    void init_vulkan();

    void init_swapchain();

    void init_commands();

    void init_default_renderpass();

    void init_framebuffers();

    void init_sync_structures();

    void init_pipelines();

    void load_meshes();

    void upload_mesh(Mesh& mesh);

};

}

#endif // ENGINE_H