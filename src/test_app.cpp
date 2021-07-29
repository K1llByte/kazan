#include "test_app.hpp"


namespace kzn
{

TestApp::TestApp()
    : window(WIDTH, HEIGHT, "GLFW Window"),
      device{window},
      pipeline(device, "shaders/shader.vert.spv", "shaders/shader.frag.spv", Pipeline::default_config(WIDTH, HEIGHT)),
      swap_chain(device, window.get_extent()) {}

void TestApp::run()
{
    while(!window.should_close())
    {
        glfwPollEvents();
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
    auto pipeline_config = Pipeline::default_config(swap_chain);
}

void TestApp::create_command_buffers()
{

}

void TestApp::draw_frame()
{

}


}