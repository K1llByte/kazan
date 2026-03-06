#pragma once

#include "GLFW/glfw3.h"
#include "core/window.hpp"
#include "ecs/system.hpp"
#include "graphics/camera.hpp"
#include "input/input.hpp"
#include "input/keyboard.hpp"
#include "input/mouse.hpp"
#include "math/types.hpp"

namespace kzn {

struct CameraSystem: public System {
    float default_speed = 2.f;
    float fast_speed = 3.f;
    float mouse_sensitivity = 0.002f;

    [[nodiscard]]
    Camera3DComponent* get_current_camera(Scene& scene) const {
        Camera3DComponent* camera3d_ptr = nullptr;
        for (auto [entity, camera] : scene.registry.registry().view<Camera3DComponent>().each()) {
            camera3d_ptr = &camera;
            break;
        }
        return camera3d_ptr;
    }

    void update(Scene& scene, float delta_time) override {
        auto& keyboard = context<Input>().keyboard();
        auto& mouse = context<Input>().mouse();

        // Find camera 3d
        Camera3DComponent* camera3d_ptr = get_current_camera(scene);
        if(camera3d_ptr == nullptr) {
            return;
        }
        auto& camera3d = *camera3d_ptr;

        // Update camera
        const float is_fast_factor = float(keyboard.is_pressed(KeyboardKey::ShiftLeft));
        const float fast_factor = default_speed + (fast_speed - default_speed) * is_fast_factor;
        if(keyboard.is_pressed(KeyboardKey::W)) {
            camera3d.position += camera3d.forward * delta_time * fast_factor;
        }
        if(keyboard.is_pressed(KeyboardKey::S)) {
            camera3d.position -= camera3d.forward * delta_time * fast_factor;
        }
        if(keyboard.is_pressed(KeyboardKey::D)) {
            camera3d.position += camera3d.right * delta_time * fast_factor;
        }
        if(keyboard.is_pressed(KeyboardKey::A)) {
            camera3d.position -= camera3d.right * delta_time * fast_factor;
        }
        if(keyboard.is_pressed(KeyboardKey::E)) {
            camera3d.position += Vec3{0,1,0} * delta_time * fast_factor;
        }
        if(keyboard.is_pressed(KeyboardKey::Q)) {
            camera3d.position -= Vec3{0,1,0} * delta_time * fast_factor;
        }

        if(mouse.is_pressed(MouseButton::Right)) {
            context<Window>().set_mouse_mode(GLFW_CURSOR_DISABLED);
            Vec2d delta_pos = mouse.delta_position();
    
            float yaw = delta_pos.x * mouse_sensitivity;
            float pitch = delta_pos.y * mouse_sensitivity;
    
            const float cos_yaw = std::cos(yaw);
            const float sin_yaw = std::sin(yaw);
            camera3d.forward = glm::normalize(Vec3{
                cos_yaw * camera3d.forward.x + sin_yaw * camera3d.forward.z,
                camera3d.forward.y,
                -sin_yaw * camera3d.forward.x + cos_yaw * camera3d.forward.z
            });

            const float cos_pitch = std::cos(pitch);
            const float sin_pitch = std::sin(pitch);
            camera3d.forward = glm::normalize(
                camera3d.forward * cos_pitch + camera3d.up * sin_pitch
            );
    
            // Rebuild orthonormal basis
            camera3d.right = glm::normalize(glm::cross(camera3d.forward, Vec3{0,-1,0}));
            camera3d.up = glm::normalize(glm::cross(camera3d.right, camera3d.forward));
        }
        else {
            context<Window>().set_mouse_mode(GLFW_CURSOR_NORMAL);
        }
    }
};

} // namespace kzn