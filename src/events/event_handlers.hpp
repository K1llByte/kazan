#pragma once

#include "events/event_manager.hpp"

namespace kzn {

template<typename T, typename E>
std::pair<std::type_index, EventHandler>
register_event_handler(T* instance, void (T::*func)(const E&)) {
    return { typeid(E), EventHandler(instance, func) };
}

// Prototype 1
//! A class that should be inherited by objects that need to register managed
//!  event handlers
class EventHandlers {
protected:
    using AutoEventHandler =
        std::pair<std::type_index, EventHandler::HandlerID>;

    EventHandlers(std::initializer_list<
                  std::pair<std::type_index, EventHandler>> _auto_handlers) {
        managed_handlers.reserve(_auto_handlers.size());

        // For each managed_handler attach to EventManager
        for (auto& [type_idx, handler] : _auto_handlers) {
            managed_handlers.emplace_back(type_idx, handler.id());
            EventManager::listen(type_idx, handler);
        }
    }

    ~EventHandlers() {
        // For each managed_handler detach from EventManager
        for (auto [type_idx, handler_id] : managed_handlers) {
            EventManager::unlisten(type_idx, handler_id);
        }
    }

private:
    std::vector<AutoEventHandler> managed_handlers;
};

// Prototype 2
// template<auto ...Handlers>
// class EventHandlers {
//     protected:

//     template<typename Handler>
//     void tmp() {
//         auto [type_idx, handler] = register_event_handler(this, &Handler);
//         managed_handlers.emplace_back(type_idx, handler.id());
//         EventManager::listen(type_idx, handler);
//     }

//     EventHandlers()
//     {
//         // Each Handlers MUST be a member function
//         managed_handlers.reserve(sizeof...(Handlers));

//         // For each managed_handler attach to EventManager
//         // (ACTION, ...);
//         (tmp<Handler>(), ...)
//     }

//     ~EventHandlers()
//     {
//         // For each managed_handler detach from EventManager
//         for(auto [type_idx, handler_id] : managed_handlers) {
//             EventManager::inlisten(type_idx, handler_id);
//         }
//     }

//     private:
//     std::vector<AutoEventHandler> managed_handlers;
// };
} // namespace kzn