#pragma once

#include <typeindex>
#include <vector>
#include <type_traits>
#include <utility>
#include <vector>

#include "core/events/event.hpp"

namespace kzn
{
    ////////////////////////////////////////////////
    //               Event handler                //
    ////////////////////////////////////////////////

    class EventHandler {
        public:
        using HandlerID = std::uintptr_t;

        EventHandler() = default;

        template<typename T, typename E>
            requires std::is_base_of_v<Event, E>
        explicit EventHandler(T* instance, void (T::*func)(const E&))
            : handler_id(id_counter++)
            , handler{bind_event_handler(instance, func)}
        {

        }

        template<typename E>
            requires std::is_base_of_v<Event, E>
        explicit EventHandler(void (*func)(const E&))
            : handler_id(id_counter++)
            , handler{bind_event_handler(func)}
        {
            
        }

        EventHandler(EventHandler&&) = default;
        EventHandler& operator=(EventHandler&&) = default;
        EventHandler(const EventHandler&) = default;
        EventHandler& operator=(const EventHandler&) = default;

        ~EventHandler() = default;

        constexpr HandlerID get_id() const
        {
            return handler_id;
        }

        constexpr HandlerID get_id()
        {
            return handler_id;
        }

        // FIXME: Call operator
        template<typename E>
        void operator()(const E& e)
        {
            handler(e);
        }

        // private:
        static inline HandlerID id_counter = 0;
        HandlerID handler_id = 0;
        std::function<void(const Event&)> handler;
    };

    ////////////////////////////////////////////////
    //               Event manager                //
    ////////////////////////////////////////////////
    class EventManager {
        public:
        EventManager() = delete;
        
        template<typename E>
            requires std::is_base_of_v<Event, E>
        static void submit(const E& event)
        {
            E e = event;
            auto it = std::ranges::find_if(
                m_handlers,
                [](const auto& p) {
                    return p.first == typeid(E);
                }
            );
            if(it != m_handlers.end())
            {
                for(const auto& event_handler : it->second)
                {
                    event_handler.handler(e);
                }
            }
        }

        static void attach(std::type_index type_idx, const EventHandler& event_handler)
        {
            auto it = std::ranges::find_if(
                m_handlers,
                [type_idx](const auto& p) {
                    return p.first == type_idx;
                }
            );
            if(it != m_handlers.end())
            {
                it->second.push_back(event_handler);
            }
            else
            {
                m_handlers.emplace_back(
                    type_idx,
                    std::vector{event_handler}
                );
            }
        }

        template<typename E>
            requires std::is_base_of_v<Event, E>
        static void attach(EventHandler&& event_handler)
        {
            attach(typeid(E), std::move(event_handler));
        }

        static void detach(std::type_index type_idx, EventHandler::HandlerID handler_id)
        {
            auto it = std::ranges::find_if(
                m_handlers,
                [type_idx](const auto& p) {
                    return p.first == type_idx;
                }
            );

            auto rmit = std::remove_if(
                it->second.begin(),
                it->second.end(),
                [handler_id](EventHandler& eh) {
                    return handler_id == eh.get_id();
                }
            );
            it->second.erase(rmit, it->second.end());
        }

        private:
        using EventToHandlers = std::pair<
            std::type_index,
            std::vector<EventHandler>
        >;
        static inline std::vector<EventToHandlers> m_handlers = {};
    };

    ////////////////////////////////////////////////
    //        Event handle binding helpers        //
    ////////////////////////////////////////////////

    template<typename T, typename E>
        requires std::is_base_of_v<Event, E>
    constexpr std::function<void(const Event&)> bind_event_handler(T* instance, void (T::*func)(const E&))
    {
        return [instance, func](const Event& event) {
            return (instance->*func)(static_cast<const E&>(event));
        };
    }

    template<typename E>
        requires std::is_base_of_v<Event, E>
    constexpr std::function<void(const Event&)> bind_event_handler(void (*func)(const E&))
    {
        return [func](const Event& event) {
            return (*func)(static_cast<const E&>(event));
        };
    }
} // namespace kzn