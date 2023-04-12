#pragma once

#include "vk/swapchain.hpp"
#include "core/window.hpp"

#include <cassert>

namespace kzn
{
    class Context
    {
    public:
        ~Context() = default;

        static Context* create(Window& window);
        static void destroy();

        static Context* get()
        {
            return Context::self;
        }

        static vk::Instance& instance()
        {
            assert(Context::get() != nullptr && "Renderer must be instantiated");
            return Context::get()->m_instance;
        }

        static vk::Device& device()
        {
            assert(Context::get() != nullptr && "Renderer must be instantiated");
            return Context::get()->m_device;
        }

        static vk::Swapchain& swapchain()
        {
            assert(Context::get() != nullptr && "Renderer must be instantiated");
            return Context::get()->m_swapchain;
        }


    private:
        Context(Window& window);
        Context(const Context&) = delete;
        Context& operator=(const Context&) = delete;
        Context(Context&&) = delete;
        Context& operator=(Context&&) = delete;

    private:
        // TODO: Maybe std::optional
        inline static Context* self = nullptr;

        vk::Instance   m_instance;
        // Only here because of bad default
        // initialization problems
        VkSurfaceKHR   m_surface;
        vk::Device     m_device;
        vk::Swapchain  m_swapchain;
    };
} // namespace kzn