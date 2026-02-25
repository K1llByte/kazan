#include "core/app.hpp"
#include "core/basic_app.hpp"
#include "editor/editor_system.hpp"
#include "graphics/render_system.hpp"

using namespace kzn;

inline void create_test_level(Scene& scene) {
    auto entity = scene.registry.create();
    entity.emplace<Transform2DComponent>();

    auto camera = scene.registry.create();
    camera.add(Camera2DComponent{
        .use_viewport_aspect_ratio = true,
    });
}

struct TestApp : public BasicApp {
    TestApp() {
        // Initialize systems
        m_systems.emplace<RenderSystem>();
        // EditorSystem auto registers as dependency before RenderSystem
        m_systems.emplace<EditorSystem>(m_window, m_input, m_console);

        // Load level entities
        create_test_level(m_scene);
    }
};

KZN_CREATE_APP(TestApp)