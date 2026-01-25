#include "core/app.hpp"
#include "core/console.hpp"
#include "core/timing.hpp"
#include "core/window.hpp"
#include "ecs/context.hpp"
#include "ecs/entity.hpp"
#include "ecs/scheduler.hpp"
#include "graphics/renderer.hpp"
#include "input/input.hpp"

namespace kzn {

class BasicApp : public App {
public:
    BasicApp()
        : BasicApp("test app", 800, 600) {}

    BasicApp(std::string_view name, int width, int height)
        : m_window(name, width, height)
        , m_input(m_window)
        , m_renderer(m_window) {

        // Create commands
        m_console.create_cmd("exit", [this]() { m_window.close(); });
    }

    ~BasicApp() {}

    void run() override {
        auto executor = m_systems.build();

        // Game loop
        float accum_time = 0.f;
        while (!m_window.is_closed()) {
            // Compute delta time
            float frame_time = delta_time();
            accum_time += frame_time;

            // Update window events and input state
            m_window.poll_events();
            m_input.update_state();
            
            // Update systems
            executor.update(frame_time);
            
            // Update FPS in window title every seconds
            if (accum_time > 1.f) {
                auto title = fmt::format("FPS: {:.0f}", 1.f / frame_time);
                m_window.set_title(title);
                accum_time = 0.f;
            }
        }
    }

protected:
    Context<Window> m_window;
    Context<Input> m_input;
    Context<Console> m_console;
    Context<Renderer> m_renderer;
    Registry m_registry;
    Scheduler m_systems;
};

} // namespace kzn