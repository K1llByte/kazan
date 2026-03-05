#include "core/app.hpp"
#include "core/basic_app.hpp"
#include "editor/editor_system.hpp"
#include "graphics/render_system.hpp"
#include "graphics/renderer.hpp"
#include "graphics/stages/debug_stage.hpp"
#include "graphics/stages/gpu_planet_stage.hpp"
#include "graphics/stages/render_stage.hpp"
#include "vk/pipeline_builder.hpp"

using namespace kzn;

inline void create_test_level(Scene& scene) {
    auto entity = scene.registry.create();
    entity.emplace<Transform2DComponent>();

    auto camera = scene.registry.create();
    camera.add(Camera2DComponent{
        .use_viewport_aspect_ratio = true,
    });
}

inline void init_render_stages(RenderSystem& render_sys) {
    // render_sys.emplace_stage<SpriteStage>(
    //     render_sys.context<Renderer>(),
    //     render_sys.screen_render_pass(),
    //     render_sys.camera_dset()
    // );
    render_sys.emplace_stage<GpuPlanetStage>(
        render_sys.context<Renderer>(),
        render_sys.screen_render_pass(),
        render_sys.camera_dset()
    );
    // render_sys.emplace_stage<DebugStage>(
    //     render_sys.context<Renderer>(),
    //     render_sys.screen_render_pass(),
    //     render_sys.camera_dset()
    // );
    // NOTE: EditorSystem will inject ImguiStage by event
}

struct TestApp : public BasicApp {
    TestApp() {
        // Initialize systems
        auto& render_sys = m_systems.emplace<RenderSystem>();
        init_render_stages(render_sys);
        // EditorSystem auto registers as dependency before RenderSystem
        m_systems.emplace<EditorSystem>(m_window, m_input, m_console);

        // Load level entities
        create_test_level(m_scene);
    }
};

KZN_CREATE_APP(TestApp)