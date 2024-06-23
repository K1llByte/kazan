#include "ecs/entity.hpp"
#include "ecs/system.hpp"
#include "editor/editor_pass.hpp"
#include "editor/editor_window.hpp"
#include "graphics/passes/offscreen_pass.hpp"
#include "graphics/render_image.hpp"
#include "graphics/renderer.hpp"
#include "kazan.hpp"

#include "fmt/base.h"
#include "glm/fwd.hpp"
#include <cmath>
#include <filesystem>

using namespace kzn;

// FIXME: Temporary, make Registry a scoped singleton with Singleton
// abstraction.
struct ScopedRegistry {
    ~ScopedRegistry() { Registry::destroy_all(); }
};

struct ExampleComponent {
    float speed = 1.f;
};

struct ExampleSystem : public System {
    void update(float delta_time) override {
        static size_t counter = 0;
        ++counter;
        auto view =
            Registry::registry.view<Transform2DComponent, ExampleComponent>();
        view.each(
            [&](auto entity, Transform2DComponent& tc, ExampleComponent& ec) {
                tc.rotation =
                    std::fmod(tc.rotation + delta_time * ec.speed, 2 * M_PI);
            }
        );
    }
};

void spawn_entities() {
    const size_t num_entities = 20;
    for (size_t i = 0; i < num_entities; ++i) {
        const float stride = 2.f / (num_entities + 1);
        for (size_t j = 0; j < num_entities; ++j) {
            auto square = Registry::create();
            auto& tc = square.add_component<Transform2DComponent>();
            tc.position.x = -1.f + stride + stride * float(i);
            tc.position.y = -1.f + stride + stride * float(j);
            tc.rotation = 2.f * M_PI / (std::rand() % 100);
            tc.scale = {0.1f, 0.1f};
            square.add_component<SpriteComponent>();
            auto& ec = square.add_component<ExampleComponent>();
            ec.speed = 2.f / (std::rand() % 100) + 0.5f;
        }
    }
}

class EditorApp : public App {
public:
    EditorApp()
        : m_window("Kazan Editor", 800, 600)
        , m_renderer(m_window)
        , m_editor_window(m_window) {
        // Initialize panels
        auto& viewport_panel = m_editor_window.add_panel<ViewportPanel>();
        auto& inspector_panel =
            m_editor_window.add_panel<InspectorPanel>(m_clear_color);
        m_editor_window.add_panel<EntityListPanel>();

        // Initialize systems
        auto& render_system = m_systems.emplace<RenderSystem>();
        m_systems.emplace<ExampleSystem>();
        // Build RenderGraph
        render_system.passes.push_back(std::make_unique<OffscreenPass>(
            viewport_panel.render_image(), m_clear_color
        ));
        render_system.passes.push_back(
            std::make_unique<EditorPass>(m_editor_window)
        );

        // Create entities
        spawn_entities();
        // auto square = Registry::create();
        // square.add_component<Transform2DComponent>();
        // square.add_component<SpriteComponent>();
        // inspector_panel.inspect(square);
    }

    ~EditorApp() {}

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
    ScopedRegistry m_scoped_registry;
    SystemManager m_systems;
    Vec3 m_clear_color{0.01f, 0.01f, 0.01f};
};

///////////////////////////////////////////////////////

KZN_CREATE_APP(EditorApp)