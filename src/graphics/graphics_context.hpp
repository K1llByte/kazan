#pragma once

#include "vk/device.hpp"
#include "vk/swapchain.hpp"
#include "core/window.hpp"

namespace kzn {

// //! Singleton class for wrapping inner graphics context
// class GraphicsContext {
// public:
//     static void initialize(Window& window) {
//         m_instance = std::make_optional<vk::Instance>({
//             .extensions = window.required_extensions(),
//             .with_validation = true,
//         });
//     }
//     static void terminate();

//     static vk::Instance& instance() { return m_instance.value(); }
//     static vk::Device& device() { return m_device.value(); }
//     static vk::Swapchain& swapchain() { return m_swapchain.value(); }

// private:
//     static std::optional<vk::Instance>  m_instance;
//     static std::optional<vk::Device>    m_device;
//     static std::optional<vk::Swapchain> m_swapchain;
// };

}