#pragma once

#include <entt/entt.hpp>

#include <utility>

namespace kzn {

class Entity;

//! Singleton wrapper class for managing entities lifetime.
class Registry {
public:
    friend class Entity;
    
    static Entity create();
    static void destroy(Entity& entity);

private:
    static inline entt::registry registry;
};

//! An identifier that represents a entity
class Entity {
public:
    friend class Registry;

    // Ctor
    Entity(entt::entity raw_entity);
    // Copy
    Entity(const Entity&) = default;
    Entity& operator=(const Entity&) = default;
    // Move
    Entity(Entity&&) = default;
    Entity& operator=(Entity&&) = default;
    // Dtor
    ~Entity() = default;

    template<typename Component, typename ...Args>
    Component& add_component(Args&&... args);
    template<typename Component>
    void       remove_component();
    template<typename Component>
    Component& get_component();
    template<typename Component>
    Component* try_get_component();

    bool is_valid() const;

private:
    entt::entity m_entity;
};

//////////////// Implementation ////////////////

template<typename Component, typename ...Args>
Component& Entity::add_component(Args&&... args) {
    return Registry::registry.emplace<Component>(
        m_entity,
        std::forward<Args>(args)...
    );
}

template<typename Component>
void Entity::remove_component() {
    Registry::registry.remove<Component>(m_entity);
}

template<typename Component>
Component& Entity::get_component() {
    return Registry::registry.get<Component>(m_entity);
}

template<typename Component>
Component* Entity::try_get_component() {
    return Registry::registry.try_get<Component>(m_entity);
}

} // namespace kzn