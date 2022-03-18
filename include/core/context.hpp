#ifndef KZN_CONTEXT_HPP
#define KZN_CONTEXT_HPP

#include "vk/swapchain.hpp"
#include "core/window.hpp"

namespace kzn
{
    class Context
    {
    public:
        static Context* create(Window* window);
        static void destroy();

        static Context* get()
        {
            return Context::self;
        }

        static vk::Instance& instance()
        {
            return Context::get()->_instance;
        }
        static vk::Device& device()
        {
            return Context::get()->_device;
        }
        static vk::Swapchain& swapchain()
        {
            return Context::get()->_swapchain;
        }

        ~Context() = default;

    private:
        Context(Window* window);
        Context(const Context&) = delete;
        Context& operator=(const Context&) = delete;

    private:
        inline static Context* self = nullptr;

        vk::Instance   _instance;
        // Only here because of bad default
        // initialization problems
        VkSurfaceKHR   surface;
        vk::Device     _device;
        vk::Swapchain  _swapchain;
    };
} // namespace kzn

#endif // KZN_CONTEXT_HPP