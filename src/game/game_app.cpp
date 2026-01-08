
#include "GLFW/glfw3.h"
#include "animation/animation_system.hpp"
#include "core/app.hpp"
#include "core/console.hpp"
#include "core/log.hpp"
#include "core/timing.hpp"
#include "core/window.hpp"
#include "ecs/context.hpp"
#include "ecs/entity.hpp"
#include "editor/console_panel.hpp"
#include "editor/editor_system.hpp"
#include "events/event_manager.hpp"
#include "game/levels.hpp"
#include "game/orbit_test.hpp"
#include "game/player_system.hpp"
#include "graphics/render_system.hpp"
#include "input/input.hpp"
#include "physics/physics_system.hpp"

#include <backends/imgui_impl_glfw.h>
#include <box2d/box2d.h>

using namespace kzn;

namespace game {

class PongApp : public App {
public:
    PongApp()
        : m_window("Test Game", 800, 600)
        , m_input(m_window)
        , m_renderer(m_window) {

        // Initialize systems
        m_systems.emplace<PhysicsSystem>();
        m_systems.emplace<EditorSystem>(m_window, m_input, m_console);
        m_systems.emplace<game::PlayerSystem>(m_input);
        m_systems.emplace<AnimationSystem>();
        m_systems.emplace<RenderSystem>();

        // Load level entities
        create_physics_test_level();

        m_console.create_cmd("level_orbit", []() {
            Registry::destroy_all();
            create_orbit_test_level();
        });

        m_console.create_cmd("level_camera", []() {
            Registry::destroy_all();
            create_camera_level();
        });

        m_console.create_cmd("level_physics", []() {
            Registry::destroy_all();
            create_physics_test_level();
        });
        m_console.create_cmd("exit", [this]() {
            glfwSetWindowShouldClose(m_window.glfw_ptr(), GLFW_TRUE);
        });
    }

    ~PongApp() {}

    void run() override {
        // Game loop
        float accum_time = 0.f;
        while (!m_window.is_closed()) {
            // Compute delta time
            float frame_time = delta_time();
            accum_time += frame_time;

            // Update window events and input state
            m_window.poll_events();
            m_input.update_state();

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
    Context<Window> m_window;
    Context<Input> m_input;
    Context<Console> m_console;
    Renderer m_renderer;
    Registry m_registry;
    SystemManager m_systems;
};

} // namespace game

KZN_CREATE_APP(game::PongApp)