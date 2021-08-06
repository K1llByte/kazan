#include "test_app.hpp"


namespace kzn
{

TestApp::TestApp()
    : window(WIDTH, HEIGHT, "GLFW Window"),
      device{window},
      swap_chain(device, window.get_extent())
{
    create_pipeline_layout();
    create_pipeline();
    create_command_buffers();
}

TestApp::~TestApp()
{
    vkDestroyCommandPool(device.device(), command_pool, nullptr);
    vkDestroyPipelineLayout(device.device(), pipeline_layout, nullptr);
}

void TestApp::run()
{
    while(!window.should_close())
    {
        glfwPollEvents();
        draw_frame();
    }
}

void TestApp::create_pipeline_layout()
{
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pSetLayouts = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;
    if(vkCreatePipelineLayout(device.device(), &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS)
        throw std::runtime_error("failed to create pipeline layout");
}

void TestApp::create_pipeline()
{
    auto pipeline_config = Pipeline::default_config(swap_chain.width(), swap_chain.height());
    pipeline_config.render_pass = swap_chain.getRenderPass();
    pipeline_config.pipeline_layout = pipeline_layout;
    pipeline = std::make_unique<Pipeline>(
        device,
        "shaders/shader.vert.spv",
        "shaders/shader.frag.spv",
        pipeline_config
        //Pipeline::default_config(WIDTH, HEIGHT)
        );

}

void TestApp::create_command_buffers()
{
    // ======== Command Pool - Creation ======== //

    QueueFamilyIndices queue_family_indices = device.get_physical_queue_families();

    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
    pool_info.flags = 0; // Optional

    if (vkCreateCommandPool(device.device(), &pool_info, nullptr, &command_pool) != VK_SUCCESS)
        throw std::runtime_error("failed to create command pool!");

    // ======== Command Buffers - Creation ======== //

    command_buffers.resize(swap_chain.imageCount()); // FIXME: possible error

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = static_cast<uint32_t>(command_buffers.size());

    if (vkAllocateCommandBuffers(device.device(), &alloc_info, command_buffers.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate command buffers!");


    for (size_t i = 0 ; i < command_buffers.size() ; ++i)
    {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = 0; // Optional
        begin_info.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(command_buffers[i], &begin_info) != VK_SUCCESS)
            throw std::runtime_error("failed to begin recording command buffer!");

        // ======== Starting a render pass ======== //

        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = swap_chain.getRenderPass();
        render_pass_info.framebuffer = swap_chain.getFrameBuffer(i);

        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent = swap_chain.getSwapChainExtent();

        std::array<VkClearValue, 2> clear_values{};
        clear_values[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clear_values[1].depthStencil = { 1.0f, 0 };
        //VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        render_pass_info.clearValueCount = 2;
        render_pass_info.pClearValues = clear_values.data();

        // ======== Command Buffers - register drawing commands ======== //
        
        vkCmdBeginRenderPass(command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->get_graphics_pipeline());
        vkCmdDraw(command_buffers[i], 3, 1, 0, 0);
        vkCmdEndRenderPass(command_buffers[i]);

        if (vkEndCommandBuffer(command_buffers[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to record command buffer!");
    }
}

void TestApp::draw_frame()
{
    // Acquire an image from the swap chain //
    uint32_t img_idx;
    VkResult res = swap_chain.acquireNextImage(&img_idx);
    if(res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("failed to aquire swap chain image!");

    // Execute the command buffer //
    // Return the image to the swap chain for presentation //
    res = swap_chain.submitCommandBuffers(&command_buffers[img_idx], &img_idx);

    if(res != VK_SUCCESS)
        throw std::runtime_error("failed to present swapchain image!");
}

}