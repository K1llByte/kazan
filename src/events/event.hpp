#pragma once

#include <type_traits>

namespace kzn {

////////////////////////////////////////////////
//               Event Base                   //
////////////////////////////////////////////////

struct Event {};

template<typename E>
concept IsEvent = std::is_base_of_v<Event, E>;

////////////////////////////////////////////////
//               Core Events                  //
////////////////////////////////////////////////

struct ResizeEvent : public Event {
    // int width;
    // int height;
};

} // namespace kzn
