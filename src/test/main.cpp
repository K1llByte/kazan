#include "core/app.hpp"
#include "core/basic_app.hpp"
#include "editor/editor_system.hpp"
#include "graphics/light.hpp"
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
#include "vk/uniform.hpp"

using namespace kzn;

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

// struct Lights2 {
//     // glsl::Int count;
//     // Light data[KZN_MAX_LIGHTS];
//     Vec3 data;
// };

struct TestApp : public BasicApp {
    TestApp() {
        // Camera system
        m_systems.emplace<CameraSystem>();
        // Render system
        m_systems.emplace<RenderSystem>();
        // EditorSystem auto registers as dependency before RenderSystem
        m_systems.emplace<EditorSystem>(m_window, m_input, m_console);

        // Initialize render stages
        init_render_stages();
        // Load level entities
        create_test_level();
    }

    void create_test_level() {
        auto entity = m_scene.registry.create();
        entity.emplace<Transform2DComponent>();
    
        auto camera = m_scene.registry.create();
        auto& camera3d = camera.add(Camera3DComponent{});
        camera3d.fov_v = 100.f;
        camera3d.look_at(Vec3{0,0,3}, Vec3{0}, Vec3{0,-1,0});
    
        auto sprite = m_scene.registry.create();
        sprite.add(Transform2DComponent{
            .position = Vec2{-0.5,0},
        });
        sprite.add(SpriteComponent{});
    
        auto sprite2 = m_scene.registry.create();
        sprite2.add(Transform2DComponent{
            .position = Vec2{0.5,0},
        });
        sprite2.add(SpriteComponent{});

        auto& device = m_systems
            .get<RenderSystem>()
            .context<Renderer>()
            .device();
        auto mesh1 = m_scene.registry.create();
        mesh1.emplace<MeshComponent>(device, "models://damaged_helmet.glb");
        auto& mesh1_t = mesh1.emplace<Transform3DComponent>();
        mesh1_t.position.x = 1.5f;
        mesh1_t.position.z = 0.f;

        auto mesh2 = m_scene.registry.create();
        mesh2.emplace<MeshComponent>(device, "models://damaged_helmet.glb");
        auto& mesh2_t = mesh2.emplace<Transform3DComponent>();
        mesh2_t.position.x = -1.5f;
        mesh2_t.position.z = 0.f;

        auto mesh3 = m_scene.registry.create();
        mesh3.emplace<MeshComponent>(device, "models://damaged_helmet.glb");
        auto& mesh3_t = mesh3.emplace<Transform3DComponent>();
        mesh3_t.position.x = 0.f;
        mesh3_t.position.z = 1.5f;

        auto mesh4 = m_scene.registry.create();
        mesh4.emplace<MeshComponent>(device, "models://damaged_helmet.glb");
        auto& mesh4_t = mesh4.emplace<Transform3DComponent>();
        mesh4_t.position.x = 0.f;
        mesh4_t.position.z = -1.5f;
    }

    void init_render_stages() {
        auto& render_sys = m_systems.get<RenderSystem>();

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
};

KZN_CREATE_APP(TestApp)