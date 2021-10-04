#include "renderer.hpp"

#include <stdexcept>
#include <cassert>
#include <chrono>

// Debug
#include <iostream>

namespace kzn
{

Renderer::Renderer(Window* window, Device* device)
    : _window{window}, _device{device}
{
    init();
}


Renderer::~Renderer() {}


void Renderer::init()
{
    recreate_swap_chain();
    create_command_buffers();
}


void Renderer::cleanup()
{
    if(initialized())
    {
        for(size_t i = 0; i < _uniform_buffers.size(); ++i)
        {
            vkDestroyBuffer(_device->device(), _uniform_buffers[i], nullptr);
            vkFreeMemory(_device->device(), _uniform_buffers_memory[i], nullptr);
        }

        // Destroy Descriptor Pool
        if(_descriptor_pool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(_device->device(), _descriptor_pool, nullptr);
        }

        // Destroy Command Buffers
        vkFreeCommandBuffers(
            _device->device(),
            _device->command_pool(),
            static_cast<uint32_t>(_command_buffers.size()),
            _command_buffers.data());

        // Destroy swapchain
        delete _swap_chain;
        _swap_chain = nullptr;
    }
}


bool Renderer::initialized() const
{
    return _swap_chain != nullptr;
}


VkRenderPass Renderer::render_pass() const
{
    assert(initialized());
    assert(_swap_chain != nullptr);
    return _swap_chain->render_pass();
}


float Renderer::aspect_ratio() const
{
    assert(initialized());
    return _swap_chain->extent_aspect_ratio();
}


void Renderer::recreate_swap_chain()
{
    assert(_device != nullptr && _window != nullptr);
    auto extent = _window->get_extent();
    while(extent.width == 0 || extent.height == 0)
    {
        extent = _window->get_extent();
        glfwWaitEvents();
    }

    _device->wait_idle();

    if(_swap_chain == nullptr)
    {
        _swap_chain = new SwapChain(*_device, extent, _window->surface());
        _swap_chain->init();
    }
    else
    {
        SwapChain* old_swap_chain = _swap_chain;
        // TODO: delete old_swap_chain after swapchain recreation
        delete old_swap_chain;
        _swap_chain = new SwapChain(*_device, extent, _window->surface(), nullptr /* old_swap_chain->swap_chain() */);
        _swap_chain->init();


        // TODO: make this code work
        // if (!old_swap_chain->compare_swap_formats(*_swap_chain))
        // {
        //     throw std::runtime_error("Swap chain image(or depth) format has changed!");
        // }
    }
}


void Renderer::create_command_buffers()
{
    assert(initialized());
    _command_buffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = _device->command_pool();
    alloc_info.commandBufferCount = static_cast<uint32_t>(_command_buffers.size());

    if(vkAllocateCommandBuffers(_device->device(), &alloc_info, _command_buffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}


DescriptorPool Renderer::create_descriptor_pool(const std::vector<VkDescriptorPoolSize>& /* _pool_sizes */)
{
    const uint32_t img_count = static_cast<uint32_t>(_swap_chain->image_count());

    std::vector<VkDescriptorPoolSize> pool_sizes{
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, img_count*2 }
    };

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = pool_sizes.size();
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = 10; //img_count;

    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
    if(vkCreateDescriptorPool(_device->device(), &pool_info, nullptr, &descriptor_pool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }

    return DescriptorPool{
        .device = _device,
        .pool = descriptor_pool,
        .current_index = &_current_image_index,
        .image_count = img_count
    };
}


PipelineLayoutBuilder Renderer::pipeline_layout_builder()
{
    return PipelineLayoutBuilder{
        _device
    };
}


void Renderer::init_descriptor_pool(VkDescriptorSetLayout descriptor_set_layout)
{
    const uint32_t img_count = static_cast<uint32_t>(_swap_chain->image_count());

    // std::vector<VkDescriptorPoolSize> pool_sizes = {
	// 	{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
	// 	{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
	// 	{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
	// 	{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
	// 	{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
	// 	{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
	// 	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
	// 	{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
	// 	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
	// 	{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
	// 	{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	// };

    std::vector<VkDescriptorPoolSize> pool_sizes{
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, img_count*2 }
    };

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = pool_sizes.size();
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = 10; //img_count;

    if(vkCreateDescriptorPool(_device->device(), &pool_info, nullptr, &_descriptor_pool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }

    std::vector<VkDescriptorSetLayout> layouts(img_count, descriptor_set_layout);
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = _descriptor_pool;
    alloc_info.descriptorSetCount = layouts.size();
    alloc_info.pSetLayouts = layouts.data();

    _descriptor_sets.resize(img_count);
    if(vkAllocateDescriptorSets(_device->device(), &alloc_info, _descriptor_sets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
}


DescriptorSet Renderer::init_descriptor_set()
{
    const size_t img_count = _swap_chain->image_count();
    const size_t buffers_count = _descriptor_buffer_infos.size() / img_count;

    std::vector<VkWriteDescriptorSet> descriptor_writes;

    for(size_t i = 0; i < img_count; ++i)
    {
        descriptor_writes.resize(buffers_count);
        for(size_t j = 0; j < buffers_count; ++j)
        {
            descriptor_writes[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_writes[j].dstSet = _descriptor_sets[i];
            descriptor_writes[j].dstBinding = j;
            descriptor_writes[j].dstArrayElement = 0;

            descriptor_writes[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptor_writes[j].descriptorCount = 1;
        
            descriptor_writes[j].pBufferInfo = &_descriptor_buffer_infos[j*img_count + i];
            descriptor_writes[j].pImageInfo = nullptr; // Optional
            descriptor_writes[j].pTexelBufferView = nullptr; // Optional
        }

        vkUpdateDescriptorSets(_device->device(), descriptor_writes.size(), descriptor_writes.data(), 0, nullptr);
        descriptor_writes.clear();
    }

    DescriptorSet descriptor_set{};
    descriptor_set.current_index = &_current_image_index;
    descriptor_set.descriptor_sets = _descriptor_sets;
    return descriptor_set;

}


float Renderer::delta_time()
{
    static auto current_time = std::chrono::high_resolution_clock::now();

    auto new_time = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float, std::chrono::seconds::period>(new_time - current_time).count();
    current_time = new_time;
    return dt;
}


VkCommandBuffer Renderer::begin_frame()
{
    assert(initialized());
    if(_frame_started)
    {
        throw std::runtime_error("Can't call beginFrame while already in progress");
    }

    // Aquire next image for rendering
    auto result = _swap_chain->acquire_next_image(&_current_image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreate_swap_chain();
        return nullptr;
    }

    if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    _frame_started = true;

    auto command_buffer = _command_buffers[_current_frame_index]; //getCurrentCommandBuffer();
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if(vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    return command_buffer;
}


void Renderer::end_frame()
{
    assert(initialized());
    if(!_frame_started)
    {
        throw std::runtime_error("Can't call endFrame while frame is not in progress");
    }

    auto command_buffer = _command_buffers[_current_frame_index];
    if(vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }

    auto result = _swap_chain->submit_command_buffers(&command_buffer, &_current_image_index);
    if(result == VK_ERROR_OUT_OF_DATE_KHR
        || result == VK_SUBOPTIMAL_KHR
        || _window->was_resized())
    {
        _window->reset_resized_flag();
        recreate_swap_chain();
    }
    else if(result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    _frame_started = false;
    _current_frame_index = (_current_frame_index + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
}


void Renderer::begin_render_pass(VkCommandBuffer command_buffer)
{
    assert(initialized());
    if(!_frame_started)
    {
        throw std::runtime_error("Can't call 'begin_render_pass' if frame is not in progress");
    }

    if(command_buffer != _command_buffers[_current_frame_index])
    {
        throw std::runtime_error("Can't begin render pass on command buffer from a different frame");
    }

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = _swap_chain->render_pass();
    render_pass_info.framebuffer = _swap_chain->framebuffer(_current_image_index);

    const auto swap_chain_extent = _swap_chain->swap_chain_extent();
    render_pass_info.renderArea.offset = { 0, 0 };
    render_pass_info.renderArea.extent = swap_chain_extent;

    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
    clear_values[1].depthStencil = { 1.0f, 0 };
    render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    
    viewport.width = static_cast<float>(swap_chain_extent.width);
    viewport.height = static_cast<float>(swap_chain_extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{ {0, 0}, swap_chain_extent };
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}


void Renderer::end_render_pass(VkCommandBuffer command_buffer)
{
    assert(initialized());
    if(!_frame_started)
    {
        throw std::runtime_error("Can't call 'end_render_pass' if frame is not in progress");
    }

    if(command_buffer != _command_buffers[_current_frame_index])
    {
        throw std::runtime_error("Can't end render pass on command buffer from a different frame");
    }

    vkCmdEndRenderPass(command_buffer);
}

}
