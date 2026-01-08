#pragma once

#include "core/assert.hpp"
#include "vk/instance.hpp"

#include "vk_mem_alloc.h"

#include <cstdint>
#include <functional>
#include <optional>
#include <utility>
#include <vector>
#include <vulkan/vulkan.h>

namespace kzn::vk {

struct QueueFamilies {
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool is_complete() const {
        return graphics_family.has_value() && present_family.has_value();
    }
};

struct SwapchainSupport {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;

    VkSurfaceFormatKHR select_format() const;
    VkPresentModeKHR select_present_mode() const;
    VkExtent2D select_extent(VkExtent2D extent) const;
};

//! In Vulkan, we can’t delete any object until we are sure that the GPU isn’t
//! using it. Therefore we use this deletion queue abstraction to schedule
//! deletion of vulkan objects when they are no longer in use.
//!
//! Since we need to explicitly flush the deleters, this deletion queue provides
//! a mechanism to flush all deleters at once or to flush expired deleters only.
//! The way we implement this expiration is through an explicit timer counter.
class DeletionQueue {
public:
    using DeleterFn = std::function<void()>;

    // Ctor
    DeletionQueue() = default;
    // Dtor
    ~DeletionQueue() = default;

    //! Push back deleter

    void enqueue(DeleterFn&& deleter) {
        m_deleters_queue.emplace_back(m_timer, std::move(deleter));
    }

    //! Flush all deleters
    void flush() {
        for (auto& time_deleter : m_deleters_queue) {
            std::invoke(time_deleter.second);
        }

        m_deleters_queue.clear();
    }

    //! Flush expired deleters
    void flush_expired() {
        // Erase expired deleters
        std::erase_if(m_deleters_queue, [this](auto& time_deleter) {
            if (time_deleter.first < (m_timer - m_expiration)) {
                // Flush before erase
                std::invoke(time_deleter.second);
                return true;
            }
            return false;
        });
    }

    //! If deleter timer is behind
    void set_expiration(std::int64_t expiration) {
        KZN_ASSERT(expiration > 0);
        m_expiration = expiration;
    }

    //! Increment internal timer
    void increment_timer() { m_timer += 1; }

private:
    // std::deque<std::pair<std::int64_t, DeleterFn>> m_deleters_queue;
    std::vector<std::pair<std::int64_t, DeleterFn>> m_deleters_queue;
    std::int64_t m_timer = 0;
    std::int64_t m_expiration = 0;
};

class Device;

// TODO: Condider making this a Devive subclass
struct Queue {
    Device& device;
    VkQueue vk_queue;
};

struct DeviceParams {
    std::vector<char const*> extensions = {};
    VkPhysicalDeviceFeatures features = {};
    VkSurfaceKHR surface = VK_NULL_HANDLE;
};

class Device {
public:
    // Ctor
    Device(Instance& instance, DeviceParams&& params = {});
    // Copy
    Device(Device const&) = delete;
    Device& operator=(Device const&) = delete;
    // Move
    Device(Device&&) = delete;
    Device& operator=(Device&&) = delete;
    // Dtor
    ~Device();

    // Getters
    [[nodiscard]]
    constexpr Queue graphics_queue() {
        return Queue{*this, m_vk_graphics_queue};
    }

    [[nodiscard]]
    constexpr Queue present_queue() {
        return Queue{*this, m_vk_present_queue};
    }

    [[nodiscard]]
    const SwapchainSupport& swapchain_support() const {
        return m_swapchain_support;
    }

    [[nodiscard]]
    const QueueFamilies& queue_families() const {
        return m_queue_families;
    }

    [[nodiscard]]
    VmaAllocator allocator() {
        return m_vma_allocator;
    }

    [[nodiscard]]
    DeletionQueue& main_deletion_queue() {
        return m_main_deletion_queue;
    }

    operator VkDevice() const { return m_vk_device; }
    operator VkPhysicalDevice() const { return m_vk_physical_device; }

    [[nodiscard]]
    const SwapchainSupport& find_swapchain_support(VkSurfaceKHR surface);

    void wait_idle() {
        vkDeviceWaitIdle(m_vk_device);
        m_main_deletion_queue.flush();
    }

private:
    vk::Instance& m_instance;
    VkDevice m_vk_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_vk_physical_device = VK_NULL_HANDLE;
    VkQueue m_vk_graphics_queue;
    VkQueue m_vk_present_queue;
    SwapchainSupport m_swapchain_support;
    QueueFamilies m_queue_families;
    VmaAllocator m_vma_allocator;
    DeletionQueue m_main_deletion_queue;
};

} // namespace kzn::vk