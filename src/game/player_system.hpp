#pragma once

// #define USE_PLAYER_GAMEPAD

#include "animation/sprite_animator.hpp"
#include "core/console.hpp"
#include "core/log.hpp"
#include "ecs/entity.hpp"
#include "ecs/system.hpp"
#include "graphics/render_system.hpp"
#include "graphics/sprite_component.hpp"
#include "input/input.hpp"
#include "input/keyboard.hpp"
#include "math/transform.hpp"
#include "math/types.hpp"
#include "physics/physics_system.hpp"
#include <cstdlib>

namespace game {

enum SkullAnimation {
    Idle,
    WalkingUp,
    WalkingLeft,
    WalkingDown,
    WalkingRight,
};

inline std::shared_ptr<kzn::AnimationSheet> create_skull_animations() {
    // Skull animation sheet
    auto skull_sheet = std::make_shared<kzn::AnimationSheet>(kzn::Vec2i{9, 4});
    // clang-format off
    skull_sheet->set_animations(std::vector<std::vector<kzn::TimedSlice>>{
        // Idle
        { {18, 0.1f} },
        // WalkingUp
        { {0, 0.1f},   {1, 0.1f},  {2, 0.1f},  {3, 0.1f},  {4, 0.1f},  {5, 0.1f},  {6, 0.1f},  {7, 0.1f},  {8, 0.1f} },
        // WalkingLeft
        { {9, 0.1f},  {10, 0.1f}, {11, 0.1f}, {12, 0.1f}, {13, 0.1f}, {14, 0.1f}, {15, 0.1f}, {16, 0.1f}, {17, 0.1f} },
        // WalkingDown
        { {18, 0.1f}, {19, 0.1f}, {20, 0.1f}, {21, 0.1f}, {22, 0.1f}, {23, 0.1f}, {24, 0.1f}, {25, 0.1f}, {26, 0.1f} },
        // WalkingRight
        { {27, 0.1f}, {28, 0.1f}, {29, 0.1f}, {30, 0.1f}, {31, 0.1f}, {32, 0.1f}, {33, 0.1f}, {34, 0.1f}, {35, 0.1f} }
    });
    // clang-format on

    return skull_sheet;
}

struct PlayerComponent {
    SkullAnimation orientation = SkullAnimation::WalkingDown;
};

class PlayerSystem : public kzn::System {
public:
    // Ctor
    PlayerSystem(kzn::Input& input)
        : m_input_ptr(&input) {

        context<kzn::Console>().create_cmd("create_player", []() {
            // Load animations setup
            auto skull_animations = create_skull_animations();
            // Create player entity
            auto player = kzn::Registry::create();
            player.add_component<kzn::Transform2DComponent>();
            player.add_component<PlayerComponent>();
            player.add_component<kzn::SpriteComponent>(0.4f, 0.4f);
            player.add_component<kzn::SpriteAnimatorComponent>(
                kzn::SpriteAnimator(
                    skull_animations, SkullAnimation::WalkingDown
                )
            );
        });
    }
    // Copy
    PlayerSystem(const PlayerSystem&) = delete;
    PlayerSystem& operator=(const PlayerSystem&) = delete;
    // Move
    PlayerSystem(PlayerSystem&&) = delete;
    PlayerSystem& operator=(PlayerSystem&&) = delete;
    // Dtor
    ~PlayerSystem() override = default;

    void update(float delta_time) override {
        const auto& input = *m_input_ptr;
        const auto& keyboard = m_input_ptr->keyboard();

        // Player controller using physics
        for (auto [entity, transform, player, physics] :
             kzn::Registry::registry()
                 .view<
                     kzn::Transform2DComponent,
                     PlayerComponent,
                     kzn::PhysicsComponent>()
                 .each()) {
            kzn::Vec2 velocity = {0.f, 0.f};
            player.orientation = SkullAnimation::Idle;

            if (keyboard.is_pressed(kzn::KeyboardKey::ArrowRight)) {
                velocity.x = 1.f;
                player.orientation = SkullAnimation::WalkingRight;
            }

            if (keyboard.is_pressed(kzn::KeyboardKey::ArrowLeft)) {
                velocity.x += -1.f;
                player.orientation = SkullAnimation::WalkingLeft;
            }

            if (keyboard.is_pressed(kzn::KeyboardKey::ArrowUp)) {
                velocity.y = 1.f;
                player.orientation = SkullAnimation::WalkingUp;
            }

            if (keyboard.is_pressed(kzn::KeyboardKey::ArrowDown)) {
                velocity.y += -1.f;
                player.orientation = SkullAnimation::WalkingDown;
            }
            physics.set_linear_velocity(velocity);

            auto animator_ptr =
                kzn::Registry::registry().try_get<kzn::SpriteAnimatorComponent>(
                    entity
                );
            if (animator_ptr != nullptr) {
                if (animator_ptr->animator.animation() != player.orientation) {
                    animator_ptr->animator.set_animation(player.orientation);
                }
            }
        }

        // const auto& gamepad = input.gamepad(kzn::GamepadId::Gamepad1);

        // float speed = gamepad.axis(kzn::GamepadAxis::RightTrigger) > 0.1f
        //                   ? 0.8f
        //                   : 0.6f;

        // player.orientation = SkullAnimation::Idle;

        // float vertical_move = -gamepad.axis(kzn::GamepadAxis::LeftAxisY);
        // if (std::abs(vertical_move) < 0.1f) {
        //     vertical_move = 0.f;
        // }
        // transform.position.y += delta_time * speed * vertical_move;
        // if (vertical_move > 0.f) {
        //     player.orientation = SkullAnimation::WalkingDown;
        // }
        // else if (vertical_move < 0.f) {
        //     player.orientation = SkullAnimation::WalkingUp;
        // }

        // float horizontal_move = gamepad.axis(kzn::GamepadAxis::LeftAxisX);
        // if (std::abs(horizontal_move) < 0.1f) {
        //     horizontal_move = 0.f;
        // }
        // transform.position.x += delta_time * speed * horizontal_move;
        // if (horizontal_move > 0.f) {
        //     player.orientation = SkullAnimation::WalkingRight;
        // }
        // else if (horizontal_move < 0.f) {
        //     player.orientation = SkullAnimation::WalkingLeft;
        // }

        // Temporary Debug Camera controller
        for (auto [entity, transform, camera] :
             kzn::Registry::registry()
                 .view<kzn::Transform2DComponent, kzn::Camera2DComponent>()
                 .each()) {
            const auto& keyboard = m_input_ptr->keyboard();

            constexpr float camera_speed = 0.4f;

            if (keyboard.is_pressed(kzn::KeyboardKey::W)) {
                transform.position.y -= delta_time * camera_speed;
            }
            if (keyboard.is_pressed(kzn::KeyboardKey::S)) {
                transform.position.y += delta_time * camera_speed;
            }
            if (keyboard.is_pressed(kzn::KeyboardKey::D)) {
                transform.position.x += delta_time * camera_speed;
            }
            if (keyboard.is_pressed(kzn::KeyboardKey::A)) {
                transform.position.x -= delta_time * camera_speed;
            }
            if (keyboard.is_pressed(kzn::KeyboardKey::R)) {
                transform.rotation += camera_speed * delta_time;
            }

            constexpr float zoom_speed = 0.5f;
            if (keyboard.is_pressed(kzn::KeyboardKey::E)) {
                camera.zoom += zoom_speed * delta_time;
            }
            if (keyboard.is_pressed(kzn::KeyboardKey::Q)) {
                camera.zoom -= zoom_speed * delta_time;
            }
        }
    }

private:
    kzn::Input* m_input_ptr;
};

} // namespace game