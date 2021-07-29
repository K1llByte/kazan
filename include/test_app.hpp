#ifndef TEST_APP
#define TEST_APP

#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include <memory>

namespace kzn
{

class TestApp
{

private:
    Window window;
    Device device;
    // Pipeline pipeline;
    SwapChain swap_chain;
    std::unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipeline_layout;
    std::vector<VkCommandBuffer> command_buffers;

public:
    static const int WIDTH = 800;
    static const int HEIGHT = 800;

    TestApp();
    ~TestApp();
    
    TestApp(const TestApp&) = delete;
    void operator=(const TestApp&) = delete;

    void run();
    
private:
    void create_pipeline_layout();
    void create_pipeline();
    void create_command_buffers();
    void draw_frame();
};

}

#endif // TEST_APP