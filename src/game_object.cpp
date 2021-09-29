#include "game_object.hpp"

// Debug
#include <iostream>

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


GameObject::GameObject(GameObject&& old_game_object)
    : transform{old_game_object.transform}
    , _id{old_game_object._id}
{
    model = old_game_object.model;
    old_game_object.model = nullptr;
}


GameObject::~GameObject()
{
    if(!moved())
    {
        delete model;
    }
}


bool GameObject::moved() const
{
    return model == nullptr;
}

}
