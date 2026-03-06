#pragma once

#include "core/singleton.hpp"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "entt/entity/registry.hpp"
#include <entt/entt.hpp>

#include <utility>

namespace kzn {

enum class EntityId : std::uint32_t {};

class Entity;

//! Singleton wrapper class for managing entities.
class Registry {
public:
    friend class Entity;
    Registry() = default;
    ~Registry() = default;

    entt::basic_registry<EntityId>& registry();

    [[nodiscard]]
    Entity create();

    void destroy(EntityId entity_id);

    void destroy_all();

    // TODO: [entity, c1, c2] = find_with<C1, C2>()

private:
    entt::basic_registry<EntityId> m_registry;
};

//! An identifier class that represents a entity
struct Entity {
    Registry* registry_ptr = nullptr;
    EntityId id = entt::null;
    
    friend class Registry;

    template<typename Component, typename... Args>
    Component& emplace(Args&&... args);

    template<typename Component>
    Component& add(Component component);

    template<typename Component>
    void remove();

    template<typename Component>
    [[nodiscard]]
    Component& get();

    template<typename Component>
    [[nodiscard]]
    Component* try_get();

    template<typename Component>
    [[nodiscard]]
    bool contains() const;

    [[nodiscard]]
    operator EntityId() {
        return id;
    }
};

//////////////// Implementation ////////////////

template<typename Component, typename... Args>
Component& Entity::emplace(Args&&... args) {
    return registry_ptr->registry().emplace<Component>(
        id, std::forward<Args>(args)...
    );
}

template<typename Component>
Component& Entity::add(Component component) {
    return registry_ptr->registry().emplace<Component>(id, std::move(component));
}

template<typename Component>
void Entity::remove() {
    registry_ptr->registry().remove<Component>(id);
}

template<typename Component>
[[nodiscard]]
Component& Entity::get() {
    return registry_ptr->registry().get<Component>(id);
}

template<typename Component>
[[nodiscard]]
Component* Entity::try_get() {
    return registry_ptr->registry().try_get<Component>(id);
}

template<typename Component>
[[nodiscard]]
bool Entity::contains() const {
    return registry_ptr->registry().try_get<Component>(id) != nullptr;
}

} // namespace kzn