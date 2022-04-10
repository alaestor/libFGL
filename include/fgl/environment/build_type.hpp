#pragma once
#ifndef FGL_ENVIRONMENT_BUILD_TYPE_HPP_INCLUDED
#define FGL_ENVIRONMENT_BUILD_TYPE_HPP_INCLUDED
#include "./libfgl_compatibility_check.hpp"

/// IMPORTANT LIMITATION
/*
Include order and location matters.

The booleans are only a valid reflection of the program state at the point of
inclusion because the `NDEBUG` symbol may be defined, un-defined, or re-defined
at any point before or after inclusion of this header which may make the
boolean state invalid (not an accurate reflection of the the macro).

For this reason, usage of `#ifdef` and `#ifndef` is recommended in translation
units which may `#define` and `#undef` the `NDEBUG` symbol frequently or with
unpredictable header ordering.
*/

// sanity check, is this needed?
#if defined(NDEBUG) && defined(DEBUG)
	#error conflicting state: both NDEBUG and DEBUG are defined simultaneously
#endif

namespace fgl {

[[maybe_unused]] inline constexpr bool debug_build{
#ifndef NDEBUG
	true
#else
	false
#endif
};

[[maybe_unused]] inline constexpr bool release_build{ not debug_build };

} // namespace fgl

#endif // FGL_ENVIRONMENT_BUILD_TYPE_HPP_INCLUDED
