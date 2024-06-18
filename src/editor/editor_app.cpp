#include "editor/editor_pass.hpp"
#include "editor/editor_window.hpp"
#include "graphics/passes/offscreen_pass.hpp"
#include "graphics/renderer.hpp"
#include "kazan.hpp"

#include "fmt/base.h"
#include "glm/fwd.hpp"

using namespace kzn;

class EditorApp : public App {
public:
    EditorApp()
        : m_window("Kazan Editor", 800, 600)
        , m_renderer(m_window)
        , m_editor_window(m_window) {
        // Initialize systems
        auto& render_system =
            m_systems.emplace<RenderSystem>(m_window, m_renderer);
        // render_system.passes.push_back(std::make_unique<OffscreenPass>(render_image));
        render_system.passes.push_back(
            std::make_unique<EditorPass>(m_editor_window)
        );

        // Initialize editor
        auto& viewport_panel = m_editor_window.add_panel<ViewportPanel>();
        render_system.passes.insert(
            render_system.passes.begin(),
            std::make_unique<OffscreenPass>(viewport_panel.render_image())
        );

        // Create entities
        auto square = Registry::create();
        square.add_component<Transform2DComponent>();
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
    Window m_window;
    Renderer m_renderer;
    EditorWindow m_editor_window;
    SystemManager m_systems;
};

///////////////////////////////////////////////////////

KZN_CREATE_APP(EditorApp)