#pragma once

#include <vulkan/vulkan.h>
#include <string>

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

// // NOTE: Can't be in std namespace since VkResult is just an integral type
// std::string to_string(VkResult result) {
//     switch (result)
//     {
//         case VK_SUCCESS: return "Command successfully completed";
//         case VK_NOT_READY: return "A fence or query has not yet completed";
//         case VK_INCOMPLETE: return "A return array was too small for the result";
//         default: return "Unknown error";
//     }
// }

class ResultError {
  public:
    // Ctor
    ResultError(VkResult result) : m_result(result) {}

    // Dtor
    ~ResultError() = default;

    constexpr VkResult result() const {
        return m_result;
    }
    std::string message() const {
        // return to_string(m_result);
        return {};
    }

  private:
    VkResult m_result;
};

} // namespace kzn::vk