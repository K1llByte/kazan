#pragma once

#include "animation/sprite_animator.hpp"
#include "core/console.hpp"
#include "ecs/entity.hpp"
#include "ecs/system.hpp"
#include "graphics/sprite_component.hpp"
#include "math/transform.hpp"
#include "math/types.hpp"

#include <random>

namespace game {

struct OrbitComponent {
    float time = 0.f;
    kzn::Vec2 radius = {1.f, 1.f};
    float rotation = 0.f;
    float speed = 1.f;
};

class TestSpritesSystem : public kzn::System {
public:
    // Ctor
    TestSpritesSystem() = default;
    // Copy
    TestSpritesSystem(const TestSpritesSystem&) = delete;
    TestSpritesSystem& operator=(const TestSpritesSystem&) = delete;
    // Move
    TestSpritesSystem(TestSpritesSystem&&) = delete;
    TestSpritesSystem& operator=(TestSpritesSystem&&) = delete;
    // Dtor
    ~TestSpritesSystem() override = default;

    void update(float delta_time) override {
        using namespace kzn;
        // Set different animation every second
        static float accum_time = 0.f;
        accum_time = std::fmod(accum_time + delta_time, 4.f);
        static std::size_t current_animation = 0;

        std::size_t next_animation = std::size_t(std::floor(accum_time));
        if (next_animation != current_animation) {
            for (auto [_entity, animator] :
                 Registry::registry().view<SpriteAnimatorComponent>().each()) {
                current_animation = next_animation;
                animator.animator.set_animation(current_animation);
            }
        }

        // Update orbit position
        for (auto [_entity, transform, orbit] :
             Registry::registry()
                 .view<Transform2DComponent, OrbitComponent>()
                 .each()) {
            // clang-format off
                const Mat3 rotation_mat{
                    std::cos(orbit.rotation), -std::sin(orbit.rotation), 0,
                    std::sin(orbit.rotation), std::cos(orbit.rotation), 0,
                    0, 0, 1
                };
                orbit.time += orbit.speed * delta_time;
                Vec3 position = rotation_mat * Vec3{
                    std::sin(orbit.time) * orbit.radius.x,
                    std::cos(orbit.time) * orbit.radius.y,
                    0
                };
                transform.position = Vec2{position.x, position.y};
            // clang-format on
        }
    }
};

} // namespace game