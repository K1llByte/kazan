#pragma once

namespace kzn
{
    ////////////////////////////////////////////////
    //               Event Base                   //
    ////////////////////////////////////////////////

    struct Event {};

    ////////////////////////////////////////////////
    //               Core Events                  //
    ////////////////////////////////////////////////

    struct ResizeEvent: public Event {
        // int width;
        // int height;
    };

} // namespace kzn