#include "core/app.hpp"
#include "core/basic_app.hpp"
#include "core/log.hpp"
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

#include "graphics/scene3d.hpp"

using namespace kzn;

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

        // auto scene3d_ptr = g_resources.load<Scene3DData>("models://damaged_helmet.glb");
        // Log::debug("Num meshes: {}", scene3d_ptr->meshes.size());
        // for(auto& mesh : scene3d_ptr->meshes) {
        //     Log::debug("Num vertices: {}", mesh.vertices.size());
        //     Log::debug("Num indices: {}", mesh.indices.size());
        // }
    }

    void create_test_level() {
        auto create_mesh = [this](auto mesh_path, auto position) {
            auto& device = m_systems
                .get<RenderSystem>()
                .context<Renderer>()
                .device();
            auto entity = m_scene.registry.create();
            entity.emplace<MeshComponent>(device, mesh_path);
            auto& transform = entity.emplace<Transform3DComponent>();
            transform.position = position;
        };

        auto create_spot_light = [this](Vec3 direction, Vec3 color) {
            constexpr Vec3 light_color = Vec3{0.8,0.2,0.2};
            constexpr float intensity = 0.5f;

            auto entity = m_scene.registry.create();
            entity.emplace<LightComponent>(
                spot_light(
                    intensity,
                    Vec3{0},
                    direction, 
                    10.f,
                    glm::radians(35.f),
                    glm::radians(40.f),
                    color
                )
            );
        };

        auto camera = m_scene.registry.create();
        auto& camera3d = camera.add(Camera3DComponent{});
        camera3d.fov_v = 100.f;
        camera3d.look_at(Vec3{0,0,3}, Vec3{0}, Vec3{0,-1,0});

        create_mesh("models://damaged_helmet.glb", Vec3{1.5f, 0.f, 0.f});
        create_mesh("models://damaged_helmet.glb", Vec3{-1.5f, 0.f, 0.f});
        create_mesh("models://damaged_helmet.glb", Vec3{0.f, 0.f, -1.5f});

        create_spot_light(Vec3{1,0,0}, Vec3{1});
        create_spot_light(Vec3{-1,0,0}, Vec3{1});
        create_spot_light(Vec3{0,0,-1}, Vec3{1});
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