#include "render_system.hpp"

#include "ecs/entity.hpp"
#include "events/event_manager.hpp"
#include "graphics/renderer.hpp"
#include "math/transform.hpp"

namespace kzn {

RenderSystem::RenderSystem(Window& window, Renderer& renderer)
    : m_renderer(renderer) {
}

RenderSystem::~RenderSystem() {
    Renderer::device().wait_idle();
}

void RenderSystem::update(float delta_time) {
    m_renderer.render_frame([&](auto& cmd_buffer) {
        for (auto& pass : passes) {
            pass->render(cmd_buffer);
        }
    });
}

} // namespace kzn