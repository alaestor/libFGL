#pragma once
#ifndef FGL_UTILITY_ENUMERATE_HPP_INCLUDED
#define FGL_UTILITY_ENUMERATE_HPP_INCLUDED
#include "../environment/libfgl_compatibility_check.hpp"

///// WARNING README
// default of T is uint32_t which will either be changed to std::size_t or
// uint_fast64_t in the future, pending adoption of a standard library bugfix:
// types larger than 32bit will probably cause compiler errors for now.

/// QUICK-START GUIDE
/*
	for (const auto& [i, v] : fgl::enumerate(a))
		assert(&v == &a[i]); // mutable reference like *begin()

	use fgl::cenumerate() for const references like *cbegin()

	Returns an enumerated range from fgl::zip where the first range of the
	zip is an incrementing counter of integral type `T`.
*/
/// EXAMPLE PROGRAM
/*
	#include <array>
	#include <vector>
	#include <iostream>

	#include <fgl/utility/enumerate.hpp>

	int main()
	{
		const std::array arr{ 1,2,3,4 };
		std::vector<float> vec(8, 3.14f);

		for (const auto& [i, v1, v2] : fgl::enumerate<unsigned int>(arr, vec))
		{
			std::cout << "arr" << '[' << i << "] == " << v1 << '\n';
			std::cout << "vec" << '[' << i << "] == " << v2 << '\n';
			std::cout << std::endl;
		}
	}
*/
/// EXAMPLE OUTPUT
/*
arr[0] == 1
vec[0] == 3.14

arr[1] == 2
vec[1] == 3.14

arr[2] == 3
vec[2] == 3.14

arr[3] == 4
vec[3] == 3.14
*/

#include <cstdint> // uint_fast64_t
#include <concepts> // integral
#include <limits> // numeric_limits
#include <ranges> // forward_range

#include "../utility/zip.hpp"

/// TODO once libc bug is fixed, rework test with a second type
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=100639

namespace fgl {

/*Returns a counted range iterating multiple ranges of differing types.
Example usage: `for (const auto& [i,v] : enumerate(a)) assert(&v == &a[i]);`*/
template <std::integral T = uint32_t>
constexpr auto enumerate(std::ranges::forward_range auto&& ... args)
{
	using iota_t = std::ranges::iota_view<T, T>;
	static_assert(std::ranges::sized_range<iota_t>); // could remove in future
	static_assert(std::ranges::forward_range<iota_t>); // paranoid because bug
	return fgl::zip(
		iota_t(0, std::numeric_limits<T>::max()),
		std::forward<decltype(args)>(args)...
	);
}

/*Returns a counted range iterating multiple const ranges of differing types.
Example usage: `for (const auto& [i,v] : cenumerate(a)) assert(&v == &a[i]);`*/
template <std::integral T = uint32_t>
constexpr auto cenumerate(std::ranges::forward_range auto&& ... args)
{
	using iota_t = std::ranges::iota_view<T, T>;
	static_assert(std::ranges::sized_range<iota_t>); // could remove in future
	static_assert(std::ranges::forward_range<iota_t>); // paranoid because bug
	return fgl::czip(
		iota_t(0, std::numeric_limits<T>::max()),
		std::forward<decltype(args)>(args)...
	);
}

}// namespace fgl

#endif // FGL_UTILITY_ENUMERATE_HPP_INCLUDED
