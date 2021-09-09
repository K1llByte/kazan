#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "model.hpp"

namespace kzn
{

// struct TransformComponent {
//   glm::vec2 translation{};  // (position offset)
//   glm::vec2 scale{1.f, 1.f};
//   float rotation;

//   glm::mat2 mat2() {
//     const float s = glm::sin(rotation);
//     const float c = glm::cos(rotation);
//     glm::mat2 rotMatrix{{c, s}, {-s, c}};

//     glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}};
//     return rotMatrix * scaleMat;
//   }
// };

class GameObject
{
public:

    using id_t = unsigned int;
    Model* model{};

private:

    id_t _id;
//   TransformComponent transform2d{};

public:

    static GameObject create_game_object();    
    id_t id() const;

private:

    GameObject(id_t obj_id);

};

}

#endif // GAME_OBJECT_H
