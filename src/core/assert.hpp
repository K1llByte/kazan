#pragma once

#ifndef KZN_DISABLE_ASSERT

#include "core/log.hpp"

#include <cassert>

#define KZN_ASSERT_MSG(expression, msg, ...)                                   \
    if (!(expression)) {                                                       \
        kzn::Log::error("Failed assert: " msg __VA_OPT__(, ) __VA_ARGS__);     \
    }                                                                          \
    assert(expression)

#define KZN_ASSERT(expression)                                                 \
    if (!(expression)) {                                                       \
        kzn::Log::error("Failed assert: '" #expression "'");                   \
    }                                                                          \
    assert(expression)

#else

#define KZN_ASSERT_MSG(...)

#define KZN_ASSERT(...)

#endif // KZN_DISABLE_ASSERT