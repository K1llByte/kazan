#include "glm/fwd.hpp"
#include "graphics/graphics_context.hpp"
#include "kazan.hpp"
#include <chrono>
#include <thread>
#include <type_traits>

#include "editor_render_system.hpp"
#include "editor_window.hpp"

using namespace kzn;

class EditorApp : public App {
public:
    EditorApp()
        : m_window("Kazan Editor", 800, 600) {

        // Initialize systems
        m_systems.emplace<EditorRenderSystem>(m_window);

        // NOTE: Has to be done after initializing EditorRenderSystem.
        m_window.set_theme();

        // Create entities
        // auto square = Registry::create();
        // auto& square_transform =
        // square.add_component<Transform2DComponent>();
    }

    ~EditorApp() = default;

    void run() override {
        // Game loop
        float accum_time = 0.f;
        while (!m_window.is_closed()) {
            // Update systems
            float frame_time = delta_time();
            accum_time += frame_time;

            // Update window events
            m_window.poll_events();

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

private:
    EditorWindow m_window;
    SystemManager m_systems;
};

///////////////////////////////////////////////////////

KZN_CREATE_APP(EditorApp)