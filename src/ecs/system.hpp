#pragma once

#include "ecs/context.hpp"

#include <memory>
#include <vector>

namespace kzn {

//! Base class for systems implementations.
struct System {
    virtual ~System() = default;

    virtual void update(float delta_time) = 0;

    template<typename T>
    [[nodiscard]]
    T& context() {
        return Context<T>::get();
    }
};

//! System Manager class that owns and updates systems.
//! Also responsible for scheduling the update of each system properly .
class SystemManager {
public:
    // Ctor
    SystemManager() = default;
    // Copy
    SystemManager(const SystemManager&) = delete;
    SystemManager& operator=(const SystemManager&) = delete;
    // Move
    SystemManager(SystemManager&&) = default;
    SystemManager& operator=(SystemManager&&) = default;
    // Dtor
    ~SystemManager() = default;

    void update(float delta_time) {
        for (auto& system : m_systems) {
            system->update(delta_time);
        }
    }

    template<typename S, typename... Args>
        requires std::is_base_of_v<System, S>
    S& emplace(Args&&... args) {
        m_systems.push_back(std::make_unique<S>(std::forward<Args>(args)...));
        return static_cast<S&>(*m_systems.back());
    }

private:
    std::vector<std::unique_ptr<System>> m_systems;
};

} // namespace kzn