#pragma once

#include "core/assert.hpp"
#include "core/log.hpp"
#include "math/types.hpp"
#include <algorithm>
#include <bits/ranges_algo.h>
#include <cmath>
#include <cstddef>
#include <memory>
#include <ranges>
#include <type_traits>
#include <utility>
#include <vector>

namespace kzn {

struct InvalidSpriteAnimation {};

//! Slice index and slice time pair
struct TimedSlice {
    std::size_t index;
    float time;
};
//! Container of timed slices
using SpriteAnimation = std::vector<TimedSlice>;

class AnimationSheet {
public:
    AnimationSheet(Vec2i slices)
        : m_slices{slices} {}

    ~AnimationSheet() = default;

    template<std::ranges::sized_range R>
        requires std::is_same_v<typename R::value_type, std::vector<TimedSlice>>
    void set_animations(const R& animations) {
        KZN_ASSERT_MSG(
            [&]() {
                const std::size_t max_index = m_slices.x * m_slices.y;
                for (const std::vector<TimedSlice>& animation : animations) {
                    if (animation.empty()) {
                        return false;
                    }

                    for (const TimedSlice& timed_slice : animation) {
                        if (timed_slice.index >= max_index) {
                            return false;
                        }
                    }
                }

                return true;
            }(),
            "Invalid animation slices indices"
        );

        m_animations = animations;
    }

    [[nodiscard]]
    SpriteAnimation& animation(std::size_t index) {
        KZN_ASSERT_MSG(index < m_animations.size(), "Invalid animation index");
        return m_animations[index];
    }

    [[nodiscard]]
    float max_animation_time(std::size_t index) {
        return std::ranges::fold_left(
            animation(index),
            0.f,
            [](float accum, const TimedSlice& slice) {
                return accum + slice.time;
            }
        );
    }

    [[nodiscard]]
    std::pair<Vec2, Vec2> slice(std::size_t index, float time) {
        auto& current_anim = animation(index);
        KZN_ASSERT(current_anim.size() > 0);

        time = std::min(time, max_animation_time(index));

        std::size_t selected_slice = current_anim[0].index;
        for (auto& [index, duration] : current_anim) {
            time -= duration;
            if (time <= 0.f) {
                selected_slice = index;
                break;
            }
        }

        Vec2 sprite_offset = {
            float(selected_slice % m_slices.x) / m_slices.x,
            float(selected_slice / m_slices.x) / m_slices.y
        };
        Vec2 sprite_size = {1.f / m_slices.x, 1.f / m_slices.y};

        return {sprite_offset, sprite_size};
    }

private:
    Vec2i m_slices;
    std::vector<SpriteAnimation> m_animations;
};

class SpriteAnimator {
public:
    SpriteAnimator(
        const std::shared_ptr<AnimationSheet>& sheet_ptr,
        std::size_t current_animation
    )
        : m_sheet_ptr{sheet_ptr}
        , m_current_anim_idx(current_animation) {}

    ~SpriteAnimator() = default;

    std::size_t animation() const { return m_current_anim_idx; }

    void set_animation(std::size_t index) {
        m_current_anim_idx = index;
        m_accum_time = 0.f;
    }

    void set_scale(float scale) {
        KZN_ASSERT_MSG(scale > 0.f, "Scale must be a positive value");
        m_animation_scale = scale;
    }

    std::pair<Vec2, Vec2> current_slice() {
        return m_sheet_ptr->slice(m_current_anim_idx, m_accum_time);
    }

    void update(float delta_time) {
        m_accum_time = std::fmod(
            m_accum_time + delta_time * m_animation_scale,
            m_sheet_ptr->max_animation_time(m_current_anim_idx)
        );
    }

private:
    std::shared_ptr<AnimationSheet> m_sheet_ptr;
    std::size_t m_current_anim_idx;
    float m_accum_time = 0.f;
    float m_animation_scale = 1.f;
};

struct SpriteAnimatorComponent {
    SpriteAnimator animator;
};

} // namespace kzn