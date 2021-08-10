#ifndef INSTANCE
#define INSTANCE

/**
 * The vulkan headers already define
 * preprocessor conditional compilation
 * rules for c++ compatability
 */

// extern "C"
// {
#ifndef GLFW_INCLUDE_VULKAN
#   include <vulkan/vulkan.h>
#endif
// }


namespace kzn
{

class InstanceBuilder
{

};

class Instance
{

};

} // kzn

#endif // INSTANCE