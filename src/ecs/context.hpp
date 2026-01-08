#pragma once

#include "core/assert.hpp"
#include "ecs/system.hpp"
#include <typeindex>
#include <unordered_map>
#include <utility>

namespace kzn {

template<typename T>
struct Context : public T {
    template<typename... Args>
    Context(Args&&... args)
        : T(std::forward<Args>(args)...) {
        instance_ptr = &static_cast<T&>(*this);
    }

    ~Context() {
        // Make data no longer available through context
        instance_ptr = nullptr;
    }

    operator T() { return *this; }

    [[nodiscard]]
    T& value() {
        return *this;
    }

    [[nodiscard]]
    const T& value() const {
        return *this;
    }

private:
    friend class System;

    [[nodiscard]]
    static T& get() {
        KZN_ASSERT_MSG(instance_ptr != nullptr, "Context data not available");
        return *instance_ptr;
    }

private:
    static inline T* instance_ptr = nullptr;
};

} // namespace kzn