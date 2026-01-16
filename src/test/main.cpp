#include "core/app.hpp"
#include "core/basic_app.hpp"
#include "graphics/render_system.hpp"
#include "graphics/sprite_component.hpp"
#include "editor/editor_system.hpp"

using namespace kzn;

inline void create_test_level() {
    auto entity = Registry::create();
    entity.add_component<Transform2DComponent>();
    entity.add_component<SpriteComponent>();

    auto camera = Registry::create();
    camera.add_component<Camera2DComponent>(
        Camera2DComponent{.use_viewport_aspect_ratio = true,}
    );
}

struct TestApp : public BasicApp {
    TestApp()
        : BasicApp() {

        // Initialize systems
        m_systems.emplace<EditorSystem>(m_window, m_input, m_console);
        m_systems.emplace<RenderSystem>();

        // Load level entities
        create_test_level();
    }
};

KZN_CREATE_APP(TestApp)