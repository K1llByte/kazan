#pragma once

#include "graphics/renderer.hpp"
#include "graphics/stages/render_stage.hpp"
#include "vk/functions.hpp"
#include "vk/pipeline.hpp"
#include "vk/render_pass.hpp"
#include <vulkan/vulkan_core.h>

namespace kzn {

class TestStage : public RenderStage {
public:
    // Ctor
    TestStage(Renderer& renderer, vk::RenderPass& render_pass)
        : m_renderer_ptr{&renderer}
        , m_test_pipeline{
            renderer.device(),
            vk::PipelineStages{
                .vertex = "assets/shaders/planet/planet.vert.spv",
                .fragment = "assets/shaders/planet/planet.frag.spv",
            },
            vk::PipelineConfig(render_pass)
                .set_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
                // .set_front_face(VK_FRONT_FACE_COUNTER_CLOCKWISE)
        }
    {}
    // Copy
    TestStage(const TestStage&) = delete;
    TestStage& operator=(const TestStage&) = delete;
    // Move
    TestStage(TestStage&&) = delete;
    TestStage& operator=(TestStage&&) = delete;

    void render(Scene& scene, vk::CommandBuffer& cmd_buffer) override {
        m_test_pipeline.bind(cmd_buffer);
        vk::cmd_draw(cmd_buffer, 4);
    }

private:
    Renderer* m_renderer_ptr;
    vk::Pipeline m_test_pipeline;
};

} // namespace kzn