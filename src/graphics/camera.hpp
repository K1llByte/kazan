#pragma once

#include "core/assert.hpp"
#include "core/log.hpp"
#include "math/types.hpp"

// TODO: Have a modified bool and only allow to modify camera data through
// class methods and have members private. When camera changes renderer will
// only upload data to gpu when its modified.

namespace kzn {

// NOTE: If world up is y = -1, it will be  
const Vec3 g_world_up = Vec3{0,-1,0};

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
    Vec3 up = g_world_up;
    float aspect_ratio = 1.f;
    float fov_v = 100.f;
    float near = 0.001f;
    float far = 1000.0f;
    bool use_viewport_aspect_ratio = true;

    void set_perspective(float _fov_v, float _aspect_ratio, float _near, float _far) {
        aspect_ratio = _aspect_ratio;
        fov_v = _fov_v;
        near = _near;
        far = _far;
    }

    void look_at(Vec3 _position, Vec3 target, Vec3 world_up) {
        set_view_direction(_position, target - _position, world_up);
    }

    void set_view_direction(Vec3 _position, Vec3 _forward, Vec3 world_up) {
        position = _position;
        forward = glm::normalize(_forward);
        right = glm::normalize(glm::cross(forward, world_up));
        up = glm::cross(forward, right);
    }

    [[nodiscard]]
    Mat4 projection_matrix() const {
        const float a = aspect_ratio;
        const float g = 1.0f / std::tan(glm::radians(fov_v) * 0.5);
        const float k = far / (far - near);

        return Mat4{
            {g / a, 0.0f, 0.0f,      0.0f},
            {0.0f,  g,    0.0f,      0.0f},
            {0.0f,  0.0f, k,         1.0f},
            {0.0f,  0.0f, -near * k, 0.0f},
        };
    }
    
    [[nodiscard]]
    Mat4 view_matrix() const {
        const float drp = -glm::dot(right, position);
        const float dup = -glm::dot(up, position);
        const float dfp = -glm::dot(forward, position);
        return Mat4{
            {right.x, up.x, forward.x, 0.f},
            {right.y, up.y, forward.y, 0.f},
            {right.z, up.z, forward.z, 0.f},
            {drp,     dup,  dfp,       1.f},
        };
    }
};

// //! Create perspective projection matrix
// //! \param fov_v Vertical field of view in radians
// //! \param aspect_ratio Viewport aspect ratio
// //! \param near Near plane distance
// //! \param far Far plane distance
// [[nodiscard]]
// constexpr Mat4 perspective(float fov_v, float aspect_ratio, float near, float far) {
//     const float a = aspect_ratio;
//     const float g = 1.0f / std::tan(fov_v * 0.5);
//     const float k = far / (far - near);

//     return Mat4{
//         {g / a, 0.0f, 0.0f,      0.0f},
//         {0.0f,  g,   0.0f,      0.0f},
//         {0.0f,  0.0f, k,         1.0f},
//         {0.0f,  0.0f, -near * k, 0.0f},
//     };
// }

// //! Create view matrix from camera position and target position
// [[nodiscard]]
// constexpr Mat4 look_at(Vec3 cam_position, Vec3 target, Vec3 world_up) {
//     const Vec3 forward = glm::normalize(target - cam_position);
//     const Vec3 right = glm::normalize(cross(forward, world_up));
//     const Vec3 up = glm::cross(right, forward);

//     return Mat4{
//         {right.x,    right.y,    right.z,   0.f},
//         {up.x,       up.y,       up.z,      0.f},
//         {forward.x,  forward.y,  forward.z, 0.f},
//         {0.f,        0.f,        0.f,       1.f},
//     } * Mat4{
//         {1,0,0,0},
//         {0,1,0,0},
//         {0,0,1,0},
//         {-cam_position.x,-cam_position.y,-cam_position.z,1},
//     };
// }

// //! Create view matrix from basis vectors
// [[nodiscard]]
// constexpr Mat4 view_from_basis(Vec3 position, Vec3 forward, Vec3 right, Vec3 up) {
//     return Mat4{
//         { right.x,    right.y,    right.z,    -dot(right, position) },
//         { up.x,       up.y,       up.z,       -dot(up, position) },
//         { forward.x,  forward.y,  forward.z,  -dot(forward, position) },
//         { 0.0f,       0.0f,       0.0f,       1.0f },
//     };
// }

} // namespace kzn