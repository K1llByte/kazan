#include "entity.hpp"

#include <entt/entity/registry.hpp>

namespace kzn {

entt::basic_registry<EntityId>& Registry::registry() {
    return m_registry;
}

Entity Registry::create() {
    return Entity{
        this, 
        registry().create()
    };
}

void Registry::destroy(EntityId entity_id) {
    m_registry.destroy(entity_id);
    // entity.id = entt::null;
}

void Registry::destroy_all() {
    for (auto entity : m_registry.view<EntityId>()) {
        m_registry.destroy(entity);
    }
}

} // namespace kzn