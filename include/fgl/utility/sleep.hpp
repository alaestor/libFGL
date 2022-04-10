#pragma once
#ifndef FGL_UTILITY_SLEEP_HPP_INCLUDED
#define FGL_UTILITY_SLEEP_HPP_INCLUDED
#include "../environment/libfgl_compatibility_check.hpp"

/// QUICK-START GUIDE
/*
fgl::nano_sleep(std::chrono::nanoseconds(500)); // hot loop
fgl::micro_sleep(std::chrono::microseconds(2)); // yielding loop
*/
/// NOTE
/*
Prefer the use of `std::this_thread::sleep_for` and `std::this_thread::yield`
where more appropriate.

However, there are use cases where this header's
functions may be applicable. `nano_sleep` is implemented as a nanosecond
resolution hot loop which may be used for sub-`yield` timing (`+~400ns`
under ideal conditions). `micro_sleep` is implemented as a microsecond
resolution `yield`ing loop which may be used for sub-`sleep_for` timing
(`+~3μs` under ideal conditions). The only guarentee is that the functions
will return some time after requested duration. The exact delays are subject
to the whims of the scheduler, platform, and `std::chrono::steady_clock`
implementation.

For accuracy over longer time scales, the standard provided sleep functions
can be layered on top of these functions. For example: `sleep_until` or
`sleep_for` can be used to bring the required resolution within the
appropriate timeframe for `micro_sleep`, which can be used to further reduce
the timeframe until `nano_sleep` should take over. With some testing to figure
out the ranges for the ideal timeframes, this technique can feisably provide
very good sleep accuracy on certain platforms and environments.
*/

#include <chrono>
#include <thread>

namespace fgl {

// yielding spin loop (+~ 3us under ideal conditions)
inline void micro_sleep(const std::chrono::microseconds duration) noexcept
{
	using clock = std::chrono::steady_clock;
	static_assert(clock::is_steady == true);

	for (const auto t{ clock::now() + duration }; clock::now() < t;)
		std::this_thread::yield();
}

// hot spin loop; doesn't yield (+~ 400ns under ideal conditions)
inline void nano_sleep(const std::chrono::nanoseconds duration) noexcept
{
	using clock = std::chrono::steady_clock;
	static_assert(clock::is_steady == true);

	for (const auto t{ clock::now() + duration }; clock::now() < t;);
}

} // namespace fgl

#endif // FGL_UTILITY_SLEEP_HPP_INCLUDED
