#pragma once

#include "event_manager.hpp"

namespace kzn {

//! Swapchain resize event, emitted when the Vulkan swapchain is recreated due
//! to a window resize or other surface change.
struct SwapchainResizeEvent : Event {};

//! Editor initialized event, emitted when the EditorSystem finished
//! initializing.
struct EditorInitEvent : Event {};

} // namespace kzn
