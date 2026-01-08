#pragma once

#include "animation/sprite_animator.hpp"
#include "box2d/box2d.h"
#include "box2d/id.h"
#include "box2d/math_functions.h"
#include "box2d/types.h"
#include "core/assert.hpp"
#include "core/singleton.hpp"
#include "ecs/context.hpp"
#include "ecs/entity.hpp"
#include "ecs/system.hpp"
#include "events/event_manager.hpp"
#include "graphics/debug_render.hpp"
#include "graphics/sprite_component.hpp"
#include "input/input.hpp"
#include "input/keyboard.hpp"
#include "math/transform.hpp"

namespace kzn {

struct PhysicsWorld : public Singleton<PhysicsWorld> {
    b2WorldId world_id;
};

struct PhysicsParams {
    Vec2 box_collider_size = Vec2{1.f, 1.f};
    b2BodyType body_type = b2_staticBody;
    bool fixed_rotation = true;
};

//! Convinience function to
[[nodiscard]]
inline Entity contact_entity_a(const b2ContactData& contact_data) {
    auto entity_ptr = static_cast<kzn::Entity*>(
        b2Body_GetUserData(b2Shape_GetBody(contact_data.shapeIdA))
    );
    KZN_ASSERT(entity_ptr != nullptr);
    return *entity_ptr;
}

class PhysicsComponent {
public:
    PhysicsComponent(Entity entity_id, PhysicsParams params = {})
        : m_entity_id{entity_id}
        , m_size{params.box_collider_size} {

        // Get physics world id
        const auto world_id = PhysicsWorld::singleton().world_id;

        // Create physics body
        b2BodyDef body_def = b2DefaultBodyDef();
        // body_def.fixedRotation = params.fixed_rotation;
        body_def.type = params.body_type;
        body_def.position = b2Vec2{0.0f, 0.0f};
        m_body_id = b2CreateBody(world_id, &body_def);

        // Create body shape
        b2Polygon dynamic_box = b2MakeBox(m_size.x / 2.f, m_size.y / 2.f);
        b2ShapeDef shape_def = b2DefaultShapeDef();
        shape_def.density = 1.0f;
        // shape_def.friction = 0.f;
        m_shape_id = b2CreatePolygonShape(m_body_id, &shape_def, &dynamic_box);

        // Set body user data to entity id for access on collision data
        b2Body_SetUserData(m_body_id, static_cast<void*>(&m_entity_id));
    }

    ~PhysicsComponent() {
        // TODO: Delete body and remove user data
    }

    void set_linear_velocity(Vec2 velocity) {
        b2Body_SetLinearVelocity(m_body_id, b2Vec2{velocity.x, velocity.y});
    }

    Vec2 linear_velocity() {
        auto velocity = b2Body_GetLinearVelocity(m_body_id);
        return Vec2{velocity.x, velocity.y};
    }

    std::vector<b2ContactData> body_contact_data() const {
        std::vector<b2ContactData> contact_data;
        contact_data.resize(b2Body_GetContactCapacity(m_body_id));
        b2Body_GetContactData(
            m_body_id, contact_data.data(), contact_data.size()
        );

        return contact_data;
    }

private:
    friend class PhysicsSystem;

private:
    Entity m_entity_id;
    b2BodyId m_body_id;
    b2ShapeId m_shape_id;
    Vec2 m_size;
};

class PhysicsSystem : public System {
public:
    // Ctor
    PhysicsSystem() {
        // Create physics world
        b2WorldDef world_def = b2DefaultWorldDef();
        world_def.gravity = b2Vec2{0.0f, 0.0f};
        // world_def.gravity = b2Vec2{0.0f, -5.0f};
        m_physics_world.world_id = b2CreateWorld(&world_def);
    }
    // Copy
    PhysicsSystem(const PhysicsSystem&) = delete;
    PhysicsSystem& operator=(const PhysicsSystem&) = delete;
    // Move
    PhysicsSystem(PhysicsSystem&&) = delete;
    PhysicsSystem& operator=(PhysicsSystem&&) = delete;
    // Dtor
    ~PhysicsSystem() {
        // Destroy physics world
        b2DestroyWorld(m_physics_world.world_id);
    }

    void update(float delta_time) override {
        auto& input = context<Input>();

        if (m_simulate_physics) {
            // Pre physics transform component sync
            auto view = Registry::registry()
                            .view<PhysicsComponent, Transform2DComponent>();
            for (auto [entity, physics, transform] : view.each()) {
                b2Rot rotation = b2Body_GetRotation(physics.m_body_id);
                b2Body_SetTransform(
                    physics.m_body_id,
                    b2Vec2{transform.position.x, transform.position.y},
                    rotation
                );
            }

            constexpr int sub_step_count = 4;
            b2World_Step(m_physics_world.world_id, delta_time, sub_step_count);

            for (auto [entity, physics, transform] : view.each()) {

                // Draw colliders rect colliders
                context<DebugRender>().draw_rect(
                    transform.position, physics.m_size
                );

                // Post physics transform component sync
                auto position = b2Body_GetPosition(physics.m_body_id);
                transform.position.x = position.x;
                transform.position.y = position.y;
            }

            // Testing
            // auto contact_events =
            //     b2World_GetContactEvents(PhysicsWorld::singleton().world_id);

            // if (contact_events.beginCount > 0 || contact_events.endCount > 0
            // ||
            //     contact_events.hitCount > 0) {
            //     Log::debug(
            //         "Begin Contact Events: {}", contact_events.beginCount
            //     );
            //     Log::debug("End Contact Events: {}",
            //     contact_events.endCount); Log::debug("Hit Contact Events:
            //     {}", contact_events.hitCount);
            // }
        }
    }

private:
    PhysicsWorld m_physics_world;
    bool m_simulate_physics = true;
};

} // namespace kzn