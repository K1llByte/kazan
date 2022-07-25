#ifndef KZN_CAMERA_HPP
#define KZN_CAMERA_HPP

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

namespace kzn {
    class Camera {
        public:
        Camera(const Camera&) = default;
        ~Camera() = default;
        
        static Camera orthographic(float left, float right, float top, float bottom, float near, float far);
        static Camera perspective(float fov_y, float aspect, float near, float far);

        void set_orthographic(float left, float right, float top, float bottom, float near, float far);
        void set_perspective(float fov_y, float aspect, float near, float far);

        void lookat_direction(glm::vec3 position, glm::vec3 direction, glm::vec3 up = {0.f, -1.f, 0.f});
        void lookat_target(glm::vec3 position, glm::vec3 target, glm::vec3 up = {0.f, -1.f, 0.f});
        // void set_view_yxz(glm::vec3 position, glm::vec3 rotation)

        // void bind();

        constexpr const glm::mat4& projection() const;
        constexpr const glm::mat4& view() const;

        constexpr const glm::vec3& position() const;
        constexpr const glm::vec3& direction() const;
        constexpr const glm::vec3& up() const;
        constexpr const glm::vec3& right() const;

        private:
        glm::mat4 _projection_matrix{1.f};
        glm::mat4 _view_matrix{1.f};

        glm::vec3 _position;
        glm::vec3 _direction;
        glm::vec3 _up;
        glm::vec3 _right;

        private:
        Camera() = default;
    };
}

////////////////////// Implementation //////////////////////

namespace kzn {
    constexpr const glm::mat4& Camera::projection() const {
        return _projection_matrix;
    }
    
    constexpr const glm::mat4& Camera::view() const {
        return _view_matrix;
    }
    

    constexpr const glm::vec3& Camera::position() const {
        return _position;
    }
    
    constexpr const glm::vec3& Camera::direction() const {
        return _direction;
    }
    
    constexpr const glm::vec3& Camera::up() const {
        return _up;
    }
    
    constexpr const glm::vec3& Camera::right() const {
        return _right;
    }
}

#endif // KZN_CAMERA_HPP