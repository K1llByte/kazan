#include "entity.hpp"

namespace kzn {

Entity Registry::create() {
    return Entity(registry.create());
}

void Registry::destroy(Entity& entity) {
    registry.destroy(entity.m_entity);
    // TODO: Check if destroy already does this 
    entity.m_entity = entt::null;
}

Entity::Entity(entt::entity raw_entity)
    : m_entity{raw_entity} {}

}