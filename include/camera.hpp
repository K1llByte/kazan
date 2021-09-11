#ifndef CAMERA_H
#define CAMERA_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace kzn
{

class Camera
{
private:

    glm::mat4 _projection_matrix{1.f};
    glm::mat4 _view_matrix{1.f};

public:

    void set_orthographic(float left, float right, float top, float bottom, float near, float far);
    void set_prespective(float fov_y, float aspect, float near, float far);

    void lookat_direction(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
    void lookat_target(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
    // void set_view_yxz(glm::vec3 position, glm::vec3 rotation)

    const glm::mat4& projection() const;
    const glm::mat4& view() const;

};

}

#endif // CAMERA_H
