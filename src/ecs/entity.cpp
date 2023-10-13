#include "entity.hpp"

namespace kzn {

Entity Registry::create() {
    return Entity(registry.create());
}

void Registry::destroy(Entity& entity) {
    registry.destroy(entity.m_entity);
}

Entity::Entity(entt::entity raw_entity)
    : m_entity{raw_entity} {}

}