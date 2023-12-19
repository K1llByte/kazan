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
        // auto& square_sprite =
        // square.add_component<SpriteComponent>();
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

///////////////////////////////////////////////////////

struct FooEvent : Event {};

void on_foo(const FooEvent&) {
    Log::info("on_foo!");
}

struct Foo {
    template<typename E>
        requires std::is_base_of_v<Event, E>
    void operator()(const E&) {
        Log::info("void (const E&)");
    }
};

class DummyApp : public App {
public:
    void run() override {
        // KZN_REGISTER_EVENT_HANDLER(on_foo);
        EventManager::attach<FooEvent>(EventHandler(on_foo));
        EventManager::submit(FooEvent{});

        Log::info("Hello World!");
    }
};

///////////////////////////////////////////////////////

KZN_CREATE_APP(DummyApp)