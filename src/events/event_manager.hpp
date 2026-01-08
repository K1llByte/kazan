#pragma once

#include "core/log.hpp"

#include <atomic>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>

namespace kzn {

//! Base event, used to declare event types
struct Event {};

//! Auxiliary type trait to check if a certain type is an event type.
template<typename E>
concept IsEvent = std::is_base_of_v<Event, E>;

//! Event handler ID type
using EventHandlerId = std::uint32_t;

//! Event type ID type
using EventTypeId = std::type_index;

//! The EventHandler is an event callback abstraction with additional event
//! info.
class EventHandler {
public:
    //! Object callback constructor
    template<typename T, IsEvent E>
    constexpr EventHandler(T* instance, void (T::*func)(const E&))
        : m_id(s_id_counter.fetch_add(1))
        , m_event_type_id{typeid(E)}
        , m_callback([instance, func](const Event& event) {
            return (instance->*func)(static_cast<const E&>(event));
        }) {}

    //! Function callback constructor
    template<IsEvent E>
    explicit constexpr EventHandler(void (*func)(const E&))
        : m_id(s_id_counter.fetch_add(1))
        , m_event_type_id{typeid(E)}
        , m_callback([func](const Event& event) {
            return (*func)(static_cast<const E&>(event));
        }) {}

    //! Move constructor
    EventHandler(EventHandler&&) = default;

    //! Move assignment
    EventHandler& operator=(EventHandler&&) = default;

    //! Copy constructor
    EventHandler(const EventHandler&) = default;

    //! Copy assignment
    EventHandler& operator=(const EventHandler&) = default;

    //! Destructor
    ~EventHandler() = default;

    template<IsEvent E>
    constexpr void operator()(const E& e) const {
        // NOTE: If type E is not same event type of the event of the callback
        // before type erasure, this will be undefined behaviour.
        // TODO: Assert that type id of event argument should be the same as the
        // callback one (should probably save that information in the handler).
        m_callback(e);
    }

    bool operator==(const EventHandler& other) { return m_id == other.m_id; }

    //! Get event type Id of this handler
    [[nodiscard]]
    EventTypeId event_type() const {
        return m_event_type_id;
    }

    //! Get Id of this handler
    [[nodiscard]]
    EventHandlerId id() const {
        return m_id;
    }

private:
    //! Global handler id counter
    static inline std::atomic<EventHandlerId> s_id_counter = 0;
    //! Event handler id
    EventHandlerId m_id;
    //! Event type id
    EventTypeId m_event_type_id;
    //! Event handler callback
    std::function<void(const Event&)> m_callback;
};

//! Main event manager singleton.
class EventManager {
public:
    // Deleted default constructor
    EventManager() = delete;

    //! Move constructor
    EventManager(EventManager&&) = delete;

    //! Move assignment
    EventManager& operator=(EventManager&&) = delete;

    //! Copy constructor
    EventManager(const EventManager&) = delete;

    //! Copy assignment
    EventManager& operator=(const EventManager&) = delete;

    //! Destructor
    ~EventManager() = default;

    //! Dispatch event
    template<IsEvent E>
    static void send(const E& event) {
        const auto it = s_handlers.find(typeid(E));
        if (it != s_handlers.end()) {
            for (const auto& handler : it->second) {
                handler(event);
            }
        }
    }

    //! Register an event handler from an object
    template<typename T, IsEvent E>
    static EventHandler& listen(T* instance, void (T::*func)(const E&)) {
        const EventTypeId event_id = typeid(E);

        auto handler = EventHandler(instance, func);
        auto [it, was_inserted] = s_handlers.insert({event_id, {handler}});
        if (!was_inserted) {
            it->second.push_back(std::move(handler));
        }

        return it->second[it->second.size() - 1];
    }

    //! Register an event handler from a function
    template<IsEvent E>
    static EventHandler& listen(void (*func)(const E&)) {
        const EventTypeId event_id = typeid(E);

        auto handler = EventHandler(func);
        auto [it, was_inserted] = s_handlers.insert({event_id, {handler}});
        if (!was_inserted) {
            it->second.push_back(std::move(handler));
        }

        return it->second[it->second.size() - 1];
    }

    //! Unregister an event handler
    // FIXME: This is unsafe, container could invalidate refs to handlers
    // static void unlisten(EventHandler& handler) {
    //     Log::error("Trying to unlisten to ID: {}", handler.id());
    //     auto it = s_handlers.find(handler.event_type());
    //     if (it != s_handlers.end()) {
    //         std::erase_if(it->second, [&handler](EventHandler& other) {
    //             return handler.id() == other.id();
    //         });
    //     }
    // }

    //! Unregister an event handler
    static void unlisten(EventTypeId event_id, EventHandlerId handler_id) {
        auto it = s_handlers.find(event_id);
        if (it != s_handlers.end()) {
            std::erase_if(it->second, [handler_id](EventHandler& other) {
                return handler_id == other.id();
            });
        }
    }

private:
    static inline std::unordered_map<EventTypeId, std::vector<EventHandler>>
        s_handlers = {};
};

//! Interface helper type to automatically unregister event handlers when
//! destructor is called.
//! \warning Although its convinient to use EventListener, it is a virtual base
//! class that keeps track of its handlers by using an auxiliary vector
//! container, therefore, for simpler use cases, prefer using listen and
//! unlisten from EventManager explicitly.
class EventListener {
public:
    EventListener(const EventListener&) = delete;

    EventListener& operator=(const EventListener&) = delete;

    virtual ~EventListener() {
        for (auto [event_id, handler_id] : m_handlers) {
            EventManager::unlisten(event_id, handler_id);
        }
    }

protected:
    EventListener() = default;

    EventListener(EventListener&&) = default;

    EventListener& operator=(EventListener&&) = default;

    template<typename T, IsEvent E>
    EventHandler& listen(void (T::*func)(const E&)) {
        auto& handler = EventManager::listen(static_cast<T*>(this), func);
        m_handlers.emplace_back(handler.event_type(), handler.id());
        return handler;
    }

private:
    std::vector<std::pair<EventTypeId, EventHandlerId>> m_handlers;
};

} // namespace kzn