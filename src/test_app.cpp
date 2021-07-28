#include "test_app.hpp"


namespace kzn
{

TestApp::TestApp()
    : window(WIDTH, HEIGHT, "GLFW Window"),
      device{window},
      pipeline(device, "shaders/shader.vert.spv", "shaders/shader.frag.spv", Pipeline::default_config()) {}

void TestApp::run()
{
    while(!window.should_close())
    {
        glfwPollEvents();
    }
}

}