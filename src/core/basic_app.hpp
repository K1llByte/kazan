#include "core/app.hpp"
#include "core/console.hpp"
#include "core/timing.hpp"
#include "core/window.hpp"
#include "ecs/context.hpp"
#include "ecs/entity.hpp"
#include "ecs/system.hpp"
#include "graphics/renderer.hpp"
#include "input/input.hpp"

namespace kzn {

class BasicApp : public App {
public:
    BasicApp()
        : m_window("test app", 800, 600)
        , m_input(m_window)
        , m_renderer(m_window) {

        // Create commands
        m_console.create_cmd("exit", [this]() { m_window.close(); });
    }

    ~BasicApp() {}

    void run() override {
        // Game loop
        float accum_time = 0.f;
        while (!m_window.is_closed()) {
            // Compute delta time
            float frame_time = delta_time();
            accum_time += frame_time;

            // Update window events and input state
            m_window.poll_events();
            m_input.update_state();
            // Update FPS in window title every seconds
            if (accum_time > 1.f) {
                auto title = fmt::format("FPS: {:.0f}", 1.f / frame_time);
                m_window.set_title(title);
                accum_time = 0.f;
            }

            // Update systems
            m_systems.update(frame_time);
        }
    }

protected:
    Context<Window> m_window;
    Context<Input> m_input;
    Context<Console> m_console;
    Renderer m_renderer;
    Registry m_registry;
    SystemManager m_systems;
};

} // namespace kzn