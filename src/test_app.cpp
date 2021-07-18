#include "test_app.hpp"


namespace kzn
{

TestApp::TestApp()
    : window(WIDTH, HEIGHT, "GLFW Window"),
      pipeline("shaders/shader.vert.spv", "shaders/shader.frag.spv") {}

void TestApp::run()
{
    while(!window.should_close())
    {
        glfwPollEvents();
    }
}

}