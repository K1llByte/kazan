#pragma once

#include "animation/sprite_animator.hpp"
#include "ecs/entity.hpp"
#include "ecs/system.hpp"
#include "graphics/sprite_component.hpp"
#include <cmath>
namespace kzn {

class AnimationSystem : public System {
public:
    // Ctor
    AnimationSystem() = default;
    // Copy
    AnimationSystem(const AnimationSystem&) = delete;
    AnimationSystem& operator=(const AnimationSystem&) = delete;
    // Move
    AnimationSystem(AnimationSystem&&) = delete;
    AnimationSystem& operator=(AnimationSystem&&) = delete;
    // Dtor
    ~AnimationSystem() = default;

    void update(float delta_time) override {
        auto sprites_view =
            Registry::registry().view<SpriteComponent, SpriteAnimatorComponent>(
            );

        for (auto [entity, sprite, animator] : sprites_view.each()) {
            animator.animator.update(delta_time);

            auto [offset, size] = animator.animator.current_slice();
            sprite.material()->set_slice(offset, size);
        }
    }
};

} // namespace kzn