#include "core/basic_app.hpp"
#include "graphics/render_system.hpp"
#include "graphics/sprite_component.hpp"

using namespace kzn;

namespace test {

inline void create_test_level() {
    auto entity = Registry::create();
    entity.add_component<Transform2DComponent>();
    entity.add_component<SpriteComponent>();

    auto camera = Registry::create();
    camera.add_component<Camera2DComponent>();
}

struct TestApp : public kzn::BasicApp {
    TestApp()
        : BasicApp() {

        // Initialize systems
        // m_systems.emplace<EditorSystem>(m_window, m_input, m_console);
        m_systems.emplace<RenderSystem>();

        // Load level entities
        create_test_level();
    }
};

} // namespace test

int main() {
    try {
        test::TestApp app;
        app.run();
    }
    catch (const kzn::vk::ResultError& re) {
        kzn::Log::error("Fatal Error: {}", re.message());
    }
    catch (const kzn::LoadingError& le) {
        kzn::Log::error("Loading Error: {}", le.message);
    }
    return EXIT_SUCCESS;
}