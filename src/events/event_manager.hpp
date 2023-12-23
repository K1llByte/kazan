#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <typeindex>
#include <utility>
#include <vector>

#include "events/event.hpp"

namespace kzn {
////////////////////////////////////////////////
//               Event handler                //
////////////////////////////////////////////////

class EventHandler {
public:
    using HandlerID = std::uintptr_t;

    EventHandler() = default;

    template<typename T, IsEvent E>
    explicit EventHandler(T* instance, void (T::*func)(const E&))
        : m_handler_id(id_counter++)
        , m_handler{ bind_event_handler(instance, func) } {}

    template<IsEvent E>
    explicit EventHandler(void (*func)(const E&))
        : m_handler_id(id_counter++)
        , m_handler{ bind_event_handler(func) } {}

    EventHandler(EventHandler&&) = default;
    EventHandler& operator=(EventHandler&&) = default;
    EventHandler(const EventHandler&) = default;
    EventHandler& operator=(const EventHandler&) = default;

    ~EventHandler() = default;

    [[nodiscard]] constexpr HandlerID id() const { return m_handler_id; }
    [[nodiscard]] constexpr HandlerID id() { return m_handler_id; }

    template<IsEvent E>
    void operator()(const E& e) const {
        m_handler(e);
    }

private:
    static inline HandlerID id_counter = 0;
    HandlerID m_handler_id = 0;
    std::function<void(const Event&)> m_handler;
};

////////////////////////////////////////////////
//               Event manager                //
////////////////////////////////////////////////
class EventManager {
public:
    EventManager() = delete;

    template<IsEvent E>
    static void send(const E& event) {
        E e = event;
        auto it = std::ranges::find_if(m_handlers, [](const auto& p) {
            return p.first == typeid(E);
        });

        if (it != m_handlers.end()) {
            for (const auto& event_handler : it->second) {
                event_handler(e);
            }
        }
    }

    static void
    listen(std::type_index type_idx, const EventHandler& event_handler) {
        auto it = std::ranges::find_if(m_handlers, [type_idx](const auto& p) {
            return p.first == type_idx;
        });

        if (it != m_handlers.end()) {
            it->second.push_back(event_handler);
        }
        else {
            m_handlers.emplace_back(type_idx, std::vector{ event_handler });
        }
    }

    template<IsEvent E>
    static void listen(EventHandler&& event_handler) {
        listen(typeid(E), std::move(event_handler));
    }

    static void
    unlisten(std::type_index type_idx, EventHandler::HandlerID handler_id) {
        auto it = std::ranges::find_if(m_handlers, [type_idx](const auto& p) {
            return p.first == type_idx;
        });

        auto rmit = std::remove_if(
            it->second.begin(),
            it->second.end(),
            [handler_id](EventHandler& eh) { return handler_id == eh.id(); }
        );
        it->second.erase(rmit, it->second.end());
    }

private:
    using EventToHandlers =
        std::pair<std::type_index, std::vector<EventHandler>>;
    static inline std::vector<EventToHandlers> m_handlers = {};
};

////////////////////////////////////////////////
//        Event handle binding helpers        //
////////////////////////////////////////////////

template<typename T, IsEvent E>
constexpr std::function<void(const Event&)>
bind_event_handler(T* instance, void (T::*func)(const E&)) {
    return [instance, func](const Event& event) {
        return (instance->*func)(static_cast<const E&>(event));
    };
}

template<IsEvent E>
constexpr std::function<void(const Event&)> bind_event_handler(
    void (*func)(const E&)
) {
    return [func](const Event& event) {
        return (*func)(static_cast<const E&>(event));
    };
}
} // namespace kzn