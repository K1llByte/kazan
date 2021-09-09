#include "game_object.hpp"

namespace kzn
{

GameObject GameObject::create_game_object()
{
    static id_t current_id = 0;
    return GameObject{current_id++};
}


id_t GameObject::id() const
{
    return _id;
}


GameObject::GameObject(id_t obj_id)
    : _id{obj_id} {}

}
