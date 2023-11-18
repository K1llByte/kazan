#include "kazan.hpp"
#include <ratio>
#include <thread>
#include <chrono>

using namespace kzn;

inline float delta_time() {
    static auto begin = std::chrono::high_resolution_clock::now();
    const auto end = std::chrono::high_resolution_clock::now();
    const float delta = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() ;
    begin = end;
    return delta / 1000.f;
}

struct ExampleApp: public App {
    ExampleApp()
        : m_window("Kazan Editor", 800, 600)
    {
        GraphicsContext::create(m_window);
    }

    ~ExampleApp() {
        GraphicsContext::destroy();
    }

    void run() override {
        while(!m_window.is_closed()) {
            float prev_frame_time = delta_time();
            m_window.poll_events();

            Log::trace("FPS: {:.0f}", 1 / prev_frame_time);
            m_systems.update(prev_frame_time);
        }
    }

private:
    Window        m_window;
    SystemManager m_systems;
};

KZN_CREATE_APP(ExampleApp)