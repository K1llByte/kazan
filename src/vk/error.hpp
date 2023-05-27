#pragma once

#include <vulkan/vulkan.h>

#define VK_CHECK(res)                                                          \
    {                                                                          \
        if (res != VK_SUCCESS) {                                               \
            throw vk::ResultError(res);                                        \
        }                                                                      \
    }

#define VK_CHECK_MSG(res, msg, ...)                                            \
    {                                                                          \
        if (res != VK_SUCCESS) {                                               \
            throw vk::ResultError(res);                                        \
        }                                                                      \
    }

namespace kzn::vk {

class ResultError {
  public:
    // Ctor
    ResultError(VkResult result) : m_result(result) {}

    // Dtor
    ~ResultError() = default;

    constexpr VkResult result() const {
        return m_result;
    }

  private:
    VkResult m_result;
};

} // namespace kzn::vk