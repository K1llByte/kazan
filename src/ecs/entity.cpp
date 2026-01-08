#include "entity.hpp"

namespace kzn {

entt::registry& Registry::registry() {
    return singleton().m_registry;
}

Entity Registry::create() {
    return registry().create();
}

void Registry::destroy(Entity& entity) {
    registry().destroy(entity.m_entity);
    // TODO: Check if destroy already does this
    entity.m_entity = entt::null;
}

void Registry::destroy_all() {
    for (auto entity : registry().view<entt::entity>()) {
        registry().destroy(entity);
    }
}

Entity::Entity(entt::entity raw_entity)
    : m_entity{raw_entity} {
}

} // namespace kzn