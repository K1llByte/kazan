#include "events/event.hpp"
#include "glm/fwd.hpp"
#include "kazan.hpp"
#include <thread>
#include <type_traits>

using namespace kzn;

class EditorApp : public App {
public:
    EditorApp()
        : m_window("Kazan Editor", 800, 600) {
        m_systems.emplace<RenderSystem>(m_window);
        auto square = Registry::create();
        auto& square_transform = square.add_component<Transform2DComponent>();
        // auto& square_sprite = square.add_component<SpriteComponent>();
    }

    ~EditorApp() = default;

    void run() override {
        while (!m_window.is_closed()) {
            float prev_frame_time = delta_time();
            m_window.poll_events();

            // FIXME: This crashes the engine
            // m_window.set_title(
            //   fmt::format("FPS: {:.0f}", 1.f / prev_frame_time));
            m_systems.update(prev_frame_time);
            Log::trace("FPS: {:.0f}", 1.f / prev_frame_time);
        }
    }

private:
    Window m_window;
    SystemManager m_systems;
};

//////////////////////////////////////////////////////

struct FooEvent : Event {};

void on_foo(const FooEvent&) {
    Log::info("on_foo!");
}

struct Foo {
    template<IsEvent E>
    void operator()(const E&) {
        Log::info("void (const E&)");
    }
};

class DummyApp : public App {
public:
    void run() override {
        EventManager::listen<FooEvent>(EventHandler(on_foo));
        EventManager::send(FooEvent{});

        Log::info("Hello World!");
    }
};

///////////////////////////////////////////////////////

KZN_CREATE_APP(EditorApp)