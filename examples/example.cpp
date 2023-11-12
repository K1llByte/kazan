#include "kazan.hpp"
#include <thread>

using namespace kzn;

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
            m_window.poll_events();
            
            m_systems.update(0.f);
        }
    }

private:
    Window        m_window;
    SystemManager m_systems;
};

KZN_CREATE_APP(ExampleApp)