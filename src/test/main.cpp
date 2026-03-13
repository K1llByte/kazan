#include "core/app.hpp"
#include "core/basic_app.hpp"
#include "editor/editor_system.hpp"
#include "graphics/mesh.hpp"
#include "graphics/render_system.hpp"
#include "graphics/renderer.hpp"
#include "graphics/sprite_component.hpp"
#include "graphics/stages/debug_stage.hpp"
#include "graphics/stages/geometry_stage.hpp"
#include "graphics/stages/planet_stage.hpp"
#include "graphics/stages/render_stage.hpp"
#include "graphics/camera.hpp"
#include "graphics/stages/skybox_stage.hpp"
#include "math/transform.hpp"
#include "math/types.hpp"
#include "resources/resources.hpp"
#include "test/camera_system.hpp"

using namespace kzn;

inline void create_test_level(Scene& scene) {
    auto entity = scene.registry.create();
    entity.emplace<Transform2DComponent>();

    auto camera = scene.registry.create();
    auto& camera3d = camera.add(Camera3DComponent{});
    camera3d.fov_v = 100.f;
    camera3d.look_at(Vec3{0,0,3}, Vec3{0}, Vec3{0,-1,0});

    auto sprite = scene.registry.create();
    sprite.add(Transform2DComponent{
        .position = Vec2{-0.5,0},
    });
    sprite.add(SpriteComponent{});

    auto sprite2 = scene.registry.create();
    sprite2.add(Transform2DComponent{
        .position = Vec2{0.5,0},
    });
    sprite2.add(SpriteComponent{});
}

inline void init_render_stages(RenderSystem& render_sys) {
    // render_sys.emplace_stage<SpriteStage>(
    //     render_sys.context<Renderer>(),
    //     render_sys.screen_render_pass(),
    //     render_sys.camera2d_dset()
    // );
    render_sys.emplace_stage<SkyboxStage>(
        render_sys.context<Renderer>(),
        render_sys.screen_render_pass(),
        render_sys.camera3d_dset()
    );
    render_sys.emplace_stage<GeometryStage>(
        render_sys.context<Renderer>(),
        render_sys.screen_render_pass(),
        render_sys.camera3d_dset()
    );
    // render_sys.emplace_stage<PlanetStage>(
    //     render_sys.context<Renderer>(),
    //     render_sys.screen_render_pass(),
    //     render_sys.camera3d_dset()
    // );
    // render_sys.emplace_stage<DebugStage>(
    //     render_sys.context<Renderer>(),
    //     render_sys.screen_render_pass(),
    //     render_sys.camera3d_dset()
    // );
    // NOTE: EditorSystem will inject ImguiStage by event
}

struct TestApp : public BasicApp {
    TestApp() {
        // Camera system
        // m_systems.emplace<CameraSystem>();
        
        // Render system
        auto& render_sys = m_systems.emplace<RenderSystem>();
        init_render_stages(render_sys);

        // EditorSystem auto registers as dependency before RenderSystem
        m_systems.emplace<EditorSystem>(m_window, m_input, m_console);

        
        auto& device = render_sys.context<Renderer>().device();
        auto mesh = m_scene.registry.create();
        mesh.emplace<MeshComponent>(device, "models://damaged_helmet.glb");

        Log::debug("RenderSystem exists? {}", m_systems.try_get<RenderSystem>() != nullptr);
        Log::debug("EditorSystem exists? {}", m_systems.try_get<EditorSystem>() != nullptr);
        m_systems.contains<CameraSystem>();

        // Load level entities
        create_test_level(m_scene);
    }
};

KZN_CREATE_APP(TestApp)