#ifndef KZN_VK_ERROR_HPP
#define KZN_VK_ERROR_HPP

#include "core/utils.hpp"
#include "core/log.hpp"

#ifndef GLFW_INCLUDE_VULKAN
#   include <vulkan/vulkan.h>
#endif

#include <exception>

#define VK_CHECK(res)                         \
        {                                     \
            if(res != VK_SUCCESS)             \
            {                                 \
                Log::error("(VkResult = {})", \
                    res);                     \
                throw ResultError(res);       \
            }                                 \
        }

#define VK_CHECK_MSG(res, msg, ...)           \
        {                                     \
            if(res != VK_SUCCESS)             \
            {                                 \
                Log::error(msg __VA_OPT__(,)  \
                     __VA_ARGS__);            \
                throw ResultError(res);       \
            }                                 \
        }

namespace kzn::vk
{
    // FIXME: Make this class extend std::exception and
    // implement what() with vk spec error msg
    class ResultError
    {
    public:
        ResultError(VkResult res)
            : result(res) {}
        ~ResultError() = default;

        VkResult raw() const { return result; }

    private:
        VkResult result;
    };

    // Instance
    KZN_IMPL_ERROR(NoGPUSupport, "Failed to find GPUs with Vulkan support!")
    KZN_IMPL_ERROR(NoGPUSuitable, "Failed to find a suitable GPU!")

    // Device
    KZN_IMPL_ERROR(FailedDebugMessenger, "Failed to set up debug messenger!")
    KZN_IMPL_ERROR(FailedValidationLayers, "Validation Layers not available!")

    // Swapchain
    KZN_IMPL_ERROR(SwapchainResized, "Swapchain VkImages were resized!")
    KZN_IMPL_ERROR(AcquireImageError, "Failed acquiring next image in the swapchain!")

    // Pipeline
    KZN_IMPL_ERROR(ShaderModError, "Failed to create shader module!")
    KZN_IMPL_ERROR(InvalidAttributeType, "Invalid vertex input attribute type!")
    KZN_IMPL_ERROR(FileError, "Failed to open file!")

    // RenderPass
    KZN_IMPL_ERROR(InvalidAttachmentIndex, "Attachment index provided isn't within available attachments!")
    KZN_IMPL_ERROR(ColorAndResolveNotSameSize, "Color and Resolve attachments are not que same size!")

    // DescriptorSet
    // TODO:

    // IO
    // KZN_IMPL_ERROR(FileError, "Failed to open file!")
}

#endif // KZN_VK_ERROR_HPP