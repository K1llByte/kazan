#include "kazan.hpp"
#include <chrono>
#include <thread>

using namespace kzn;

inline float delta_time() {
    static auto begin = std::chrono::high_resolution_clock::now();
    const auto end = std::chrono::high_resolution_clock::now();
    const float delta =
      std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
    begin = end;
    return delta;
}

struct EditorApp : public App {
    EditorApp()
      : m_window("Kazan Editor", 800, 600) {
        GraphicsContext::create(m_window);
        m_systems.emplace<RenderSystem>();
    }

    ~EditorApp() { GraphicsContext::destroy(); }

    void run() override {
        while (!m_window.is_closed()) {
            float prev_frame_time = delta_time();
            m_window.poll_events();

            // m_window.set_title(
            //   fmt::format("FPS: {:.0f}", 1.f / prev_frame_time));
            m_systems.update(prev_frame_time);
        }
    }

private:
    Window m_window;
    SystemManager m_systems;
};

KZN_CREATE_APP(EditorApp)