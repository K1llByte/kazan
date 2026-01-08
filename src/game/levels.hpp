#pragma once

#include "animation/sprite_animator.hpp"
#include "box2d/types.h"
#include "ecs/entity.hpp"
#include "game/orbit_test.hpp"
#include "game/player_system.hpp"
#include "graphics/render_system.hpp"
#include "graphics/sprite_component.hpp"
#include "math/transform.hpp"
#include "math/types.hpp"
#include "physics/physics_system.hpp"
#include <chrono>
#include <memory>
#include <random>

#define BEGIN_PROFILING(name)                                                  \
                                                                               \
    std::chrono::steady_clock::time_point begin_##name =                       \
        std::chrono::steady_clock::now();

#define END_PROFILING(name)                                                    \
                                                                               \
    std::chrono::steady_clock::time_point end_##name =                         \
        std::chrono::steady_clock::now();                                      \
    Log::info(                                                                 \
        "{} - {} ms",                                                          \
        #name,                                                                 \
        std::chrono::duration_cast<std::chrono::milliseconds>(                 \
            end_##name - begin_##name                                          \
        )                                                                      \
            .count()                                                           \
    );

namespace game {

inline void create_orbit_test_level() {
    using namespace kzn;
    using namespace std::string_view_literals;

    BEGIN_PROFILING(load_level);

    auto skull_animations = create_skull_animations();

    // Setup console commands
    Console console;
    console.create_cmd("entity.create", [&]() {
        // Setup random distributions to create entities
        std::random_device device;
        static std::mt19937 generator(device());
        static std::uniform_real_distribution<float> distribution1(
            0.f, 2.f * M_PI
        );
        static std::uniform_real_distribution<float> distribution2(0.5f, 1.5f);
        static std::uniform_real_distribution<float> distribution3(-1.f, 1.f);

        auto sprite = Registry::create();
        // Add sprite
        sprite.add_component<SpriteComponent>(0.5f, 0.5f);
        // Add orbit
        auto& orbit = sprite.add_component<OrbitComponent>();
        // Add transform
        sprite.add_component<Transform2DComponent>();
        // Generate random positions
        orbit.rotation = distribution1(generator);
        orbit.radius.x = distribution3(generator);
        orbit.radius.y = distribution3(generator);
        orbit.speed = distribution2(generator);
        // Add sprite animator
        sprite.add_component<SpriteAnimatorComponent>(
            SpriteAnimator(skull_animations, SkullAnimation::WalkingDown)
        );
    });

    BEGIN_PROFILING(creating_entities);
    // Spawn entities using command
    for (size_t i = 0; i < 100; ++i) {
        console.execute_cmd("entity.create");
    }
    END_PROFILING(creating_entities);

    // Spawn camera entity
    auto camera = Registry::create();
    camera.add_component<Camera2DComponent>();

    END_PROFILING(load_level);
}

inline void create_physics_test_level() {
    using namespace kzn;
    using namespace std::string_view_literals;

    BEGIN_PROFILING(level_load);

    // Auxiliary function to create wall with collider
    const auto create_wall = [](Vec2 position, Vec2 size) {
        // Wall display sprite
        auto wall_entity = Registry::create();
        wall_entity.add_component<SpriteComponent>(size.x, size.y);
        wall_entity.add_component<PhysicsComponent>(
            wall_entity, PhysicsParams{.box_collider_size = size}
        );
        auto& wall_transform =
            wall_entity.add_component<Transform2DComponent>();
        wall_transform.position = position;
    };

    // Create animation sheets
    auto skull_animations = create_skull_animations();

    // Create camera entity
    auto cam_entity = Registry::create();
    cam_entity.add_component<Camera2DComponent>();
    cam_entity.add_component<Transform2DComponent>();

    // Create player entity with physics
    auto entity = Registry::create();
    entity.add_component<PhysicsComponent>(
        entity,
        PhysicsParams{
            .box_collider_size = Vec2{0.24, 0.5},
            .body_type = b2_dynamicBody,
        }
    );
    entity.add_component<SpriteComponent>(
        0.5f,
        0.5f,
        std::make_shared<kzn::SpriteMaterial>("assets/textures/skull_sheet.png")
    );
    entity.add_component<SpriteAnimatorComponent>(
        SpriteAnimator(skull_animations, SkullAnimation::WalkingDown)
    );
    entity.add_component<Transform2DComponent>();
    entity.add_component<PlayerComponent>();

    // Create walls with physics
    create_wall(Vec2{0.0f, -0.8f}, Vec2{2.f, 0.2f});
    create_wall(Vec2{0.0f, 0.8f}, Vec2{2.f, 0.2f});

    create_wall(Vec2{1.14f, 0.f}, Vec2{0.2f, 0.2f});
    create_wall(Vec2{0.7f, 0.f}, Vec2{0.2f, 0.2f});
    create_wall(Vec2{-1.14f, 0.f}, Vec2{0.2f, 0.2f});
    create_wall(Vec2{-0.7f, 0.f}, Vec2{0.2f, 0.2f});

    END_PROFILING(level_load);
}

inline void create_camera_level() {
    // Spawn camera entity
    auto camera = kzn::Registry::create();
    camera.add_component<kzn::Camera2DComponent>();
    camera.add_component<kzn::Transform2DComponent>();
}

} // namespace game