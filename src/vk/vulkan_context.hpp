#pragma once

#include "swapchain.hpp"

namespace kzn::vk {

// //! Vulkan singleton state class with all main pieces 
// class VulkanContext {
// public:
//     static void create_instance(InstanceParams&& instance_params) {
//         m_self.m_instance = std::make_optional<Instance>(std::move(instance_params));
//     }
//     static void create_device(DeviceParams&& device_params) {
//         m_self.m_device = Device(std::move(device_params));
//     }

//     static Instance& instance() { return m_self.m_instance.value(); }
//     static Device& device() { return m_self.m_device.value(); }
//     // static void swapchain() { assert(m_self != nullptr); return m_swapchain; }
//     // static void allocator { return m_allocator; }

// private:
//     // Ctor
//     VulkanContext() = default;
//     // Copy
//     VulkanContext(const VulkanContext&) = delete;
//     VulkanContext& operator=(const VulkanContext&) = delete;
//     // Move
//     VulkanContext(VulkanContext&&) = delete;
//     VulkanContext& operator=(VulkanContext&&) = delete;
//     // Ctor
//     ~VulkanContext() = default;

// private:
//     static VulkanContext m_self;
//     // NOTE: std::optional is used as a maybe uninit struct
//     std::optional<Instance> m_instance;
//     std::optional<Device>   m_device;
//     // Swapchain m_swapchain;
//     //vk::Allocator m_allocator;
// };

} // namespace kzn::vk
