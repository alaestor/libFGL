#pragma once
#ifndef FGL_ENVIRONMENT_HARDWARE_HPP_INCLUDED
#define FGL_ENVIRONMENT_HARDWARE_HPP_INCLUDED

// formerly SYSOPT_H_INCLUDED

#if !defined(__linux__) && !defined(_WIN64)
	#error Potentially unsupported OS.
#endif

#include <cstddef> // size_t
#include <cstdint> // max_align_t
#include <new> // hardware interference sizes

namespace fgl::hardware {

//****************************************************************************/
// Hardware cache alignment sizes
// Most common modern processor L1 line sizes specified in optimized
// constructive and destructive interference size, for multithreading and good
// cache behavior. Alternatively, this can be retrieved at runtime via
// implementation dependent means.

#ifndef __cpp_lib_hardware_interference_size

[[maybe_unused]] inline constexpr std::size_t dis = 128;
[[maybe_unused]] inline constexpr std::size_t cis = 64;

// TODO runtime implementations & abstract cis/dis to be transparent?
// windows -> system logical processor info
// linux -> sysconf(_SC_LEVEL1_DCACHE_LINESIZE)
// call once, etc

#else

[[maybe_unused]] inline constexpr std::size_t dis{
	std::hardware_destructive_interference_size
};

[[maybe_unused]] inline constexpr std::size_t cis{
	std::hardware_constructive_interference_size
};

#endif

static_assert(
	dis >= sizeof(std::max_align_t),
	"fgl::hardware::dis less than max_align lower bound"
);

static_assert(
	cis >= sizeof(std::max_align_t),
	"fgl::hardware::CIS less than max_align lower bound"
);

} // namespace fgl

#endif // FGL_ENVIRONMENT_HARDWARE_HPP_INCLUDED
