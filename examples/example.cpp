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
        }
    }

private:
    Window m_window;
};

KZN_CREATE_APP(ExampleApp)