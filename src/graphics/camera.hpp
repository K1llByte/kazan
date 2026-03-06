#pragma once

#include "core/log.hpp"
#include "math/types.hpp"

// TODO: Have a modified bool and only allow to modify camera data through
// class methods and have members private. When camera changes renderer will
// only upload data to gpu when its modified.

namespace kzn {

struct Camera2DComponent {
    float zoom = 1.f;
    float aspect_ratio = 1.f;
    bool use_viewport_aspect_ratio = true;

    [[nodiscard]]
    constexpr Mat4 matrix() const {
        // Initialize with identity
        auto proj_mat = Mat4{};
        // Aspect ratio multiplier
        proj_mat[0].x = 1.f / aspect_ratio;
        return proj_mat;
    }
};

struct Camera3DComponent {
    Vec3 position = Vec3{0};
    Vec3 forward = Vec3{0,0,1};
    Vec3 right = Vec3{1,0,0};
    Vec3 up = Vec3{0,-1,0};
    float aspect_ratio = 1.f;
    float fov_v = 100.f;
    float near = 0.01f;
    float far = 1000.0f;
    bool use_viewport_aspect_ratio = true;

    void look_at(Vec3 _position, Vec3 target, Vec3 _up) {
        position = _position;
        forward = glm::normalize(target - position);
        Log::debug("forward: {} {} {}", forward.x, forward.y, forward.z);
        right = glm::normalize(glm::cross(forward, _up));
        up = glm::normalize(glm::cross(right, forward));
    }

    [[nodiscard]]
    Mat4 projection_matrix() const;
    
    [[nodiscard]]
    Mat4 view_matrix() const;
};

} // namespace kzn