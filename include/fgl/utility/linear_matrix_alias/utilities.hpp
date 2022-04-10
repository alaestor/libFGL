#pragma once
#ifndef FGL_UTILITY_LINEAR_MATRIX_ALIAS_UTILITIES_HPP_INCLUDED
#define FGL_UTILITY_LINEAR_MATRIX_ALIAS_UTILITIES_HPP_INCLUDED
#include "../../environment/libfgl_compatibility_check.hpp"

/// QUICKSTART GUIDE
/*
	// need two ranges to contain the bounds and dimensional offsets
	constexpr std::size_t dims{ 4 };
	std::array<std::size_t, dims> bounds{3,3,3,3}; // tesseract (4D cube)

	// calculate dimensional offsets
	std::array<std::size_t, dims> offsets{};
	calculate_offsets<dims, fgl::major::row>(bounds, offsets);
	// or
	auto offsets{ make_offsets<dims>(bounds) };

	buffer[matrix_to_linear(std::array{0,2,2,0}, offsets)] = 1337;


	"bounds" and "offsets" ranges must always be the same length.

	For compile-time optimizations at the cost of synthesizing more templates,
	provide the number of dimensions as a template parameter `T_dimensions`
	which must either be `0` or match the length of the range(s).
*/
/// EXAMPLE PROGRAM
/*
#include <cstddef> // size_t
#include <iostream> // cout, endl
#include <iomanip> // setw
#include <array>
#include <vector>

#include <fgl/utility/linear_matrix_alias/utilities.hpp>

void fill_buffer(auto& buffer)
{
	for (std::size_t i{}; auto& v : buffer)
		v = i++;
}

void print_2d_matrix(
	const auto& buffer, const auto& bounds, const auto& offsets)
{
	using fgl::linear_matrix_utilities::matrix_to_linear;
	for (std::size_t i{}; i < bounds[0]; ++i)
	{
		for (std::size_t j{}; j < bounds[1]; ++j)
		{
			std::cout
				<< std::setw(3)
				<< buffer[matrix_to_linear(std::array{i,j}, offsets)]
				<< ", ";
		}
		std::cout << '\n';
	}
	std::cout << std::endl;
}

void print_2d_matrix_both_majors(const auto& buffer, const auto& bounds)
{
	std::vector<std::size_t> offsets(bounds.size());

	using fgl::linear_matrix_utilities::calculate_offsets;

	std::cout << "row major:\n";
	calculate_offsets<0, fgl::major::row>(bounds, offsets);
	print_2d_matrix(buffer, bounds, offsets);

	std::cout << "column major:\n";
	calculate_offsets<0, fgl::major::column>(bounds, offsets);
	print_2d_matrix(buffer, bounds, offsets);
}

int main()
{
	std::vector<std::size_t> bounds{3,3}; // 2D square, 3x3
	std::vector<std::size_t> buffer(3*3);

	fill_buffer(buffer);
	print_2d_matrix_both_majors(buffer, bounds);

	// resize square 3x3 -> 4x4
	bounds[0] = 4; bounds[1] = 4;
	buffer.resize(4*4);

	fill_buffer(buffer);
	print_2d_matrix_both_majors(buffer, bounds);
}
*/
/// EXAMPLE OUTPUT
/*
row major:
  0,   1,   2,
  3,   4,   5,
  6,   7,   8,

column major:
  0,   3,   6,
  1,   4,   7,
  2,   5,   8,

row major:
  0,   1,   2,   3,
  4,   5,   6,   7,
  8,   9,  10,  11,
 12,  13,  14,  15,

column major:
  0,   4,   8,  12,
  1,   5,   9,  13,
  2,   6,  10,  14,
  3,   7,  11,  15,
*/

#include <cstddef> // size_t
#include <stdexcept> // out_of_range
#include <concepts>
#include <ranges>
#include <numeric> // reduce
#include <functional> // multiplies

#include "../../debug/constexpr_assert.hpp"
#include "../../types/range_constraints.hpp"
#include "../zip.hpp" // czip"

namespace fgl {

enum class major : bool
{
	column = false,
	row = true
};

namespace linear_matrix_utilities {

/*Asserts that the ranges are of equal size, and then returns that size.
If T_dimensions is `0`, asserts that the sizes of a and b are the same and
returns the size. Otherwise, asserts the sizes of a and b are
T_dimensions, and then returns T_dimensions.*/
template <std::size_t T_dimensions = 0>
[[nodiscard]] constexpr inline std::size_t assert_dimensions(
	const std::ranges::sized_range auto& a,
	const std::ranges::sized_range auto& b)
noexcept
{
	// this could be a more generic `size_t assert_same_length(T_ranges ...)`
	// probably same header as aligned cast

	if constexpr (T_dimensions == 0)
	{
		const std::size_t size{ std::ranges::size(a) };
		FGL_DEBUG_CONSTEXPR_ASSERT(size == std::ranges::size(b));
		return size;
	}
	else
	{
		FGL_DEBUG_CONSTEXPR_ASSERT(std::ranges::size(a) == T_dimensions);
		FGL_DEBUG_CONSTEXPR_ASSERT(std::ranges::size(b) == T_dimensions);
		return T_dimensions;
	}
}

/*Convert dimensional indexes using offsets to produce a `T_out` linear index.
The `std::ranges::size` of `indexes` and `offsets` must be the same.
`T_dimensions` must be either equal the size of each range, or `0` in which
case the number of dimensions will be dynamically determined by the sizes.*/
template <std::size_t T_dimensions = 0, std::integral T_out = std::size_t>
[[nodiscard]] constexpr inline T_out matrix_to_linear(
	const fgl::contiguous_range_convertible_to<T_out> auto& indexes,
	const fgl::contiguous_range_convertible_to<T_out> auto& offsets)
noexcept
{
	const std::size_t dimensions{
		assert_dimensions<T_dimensions>(indexes, offsets)
	};
	T_out index{ 0 };
	for (const auto& [i, o] : fgl::czip(dimensions, indexes, offsets))
		index += static_cast<T_out>(i) * static_cast<T_out>(o);
	return index;
}

/*Calculates and assigns dimensional offsets for the `T_major` of `bounds`.
The `std::ranges::size` of `bounds` and `out_offsets` must be the same.
`T_dimensions` must be either equal the size of each range, or `0` in which
case the number of dimensions will be dynamically determined by the sizes.*/
template
<
	std::size_t T_dimensions = 0,
	major T_major = major::row,
	fgl::random_access_range_integral T_bounds,
	fgl::random_access_range_assignable_from_range<T_bounds> T_offsets
>
constexpr inline void calculate_offsets(
	const T_bounds& bounds,
	T_offsets& out_offsets)
noexcept
{
	const std::size_t dimensions{
		assert_dimensions<T_dimensions>(bounds, out_offsets)
	};
	using out_t = std::ranges::range_value_t<T_bounds>;
	if constexpr (T_major == major::row)
	{
		for (std::size_t i{ 1 }; i < dimensions; ++i)
		{
			out_offsets[i-1] = std::reduce(
				&bounds[i],
				&bounds[dimensions],
				out_t{ 1 },
				std::multiplies<out_t>()
			);
		}
		out_offsets[dimensions-1] = 1;
	}
	else // if (T_major == major::column)
	{
		out_offsets[0] = 1;
		for (std::size_t i{ 1 }; i < dimensions; ++i)
		{
			out_offsets[i] = std::reduce(
				&bounds[0],
				&bounds[i],
				out_t{ 1 },
				std::multiplies<out_t>()
			);
		}
	}
}

/* Helper function: returns a `std::array` of `T_out` containing dimensional
offsets for `T_major` of `bounds.
`T_dimensions` must equal the size of `bounds`.*/
template
<
	std::size_t T_dimensions,
	major T_major = major::row,
	typename T_out = std::size_t
>
[[nodiscard]] constexpr inline std::array<T_out, T_dimensions> make_offsets(
	const fgl::random_access_range_integral auto& bounds)
noexcept
{
	std::array<T_out, T_dimensions> array_out;
	calculate_offsets<T_dimensions, T_major>(bounds, array_out);
	return array_out;
}

// returns true if any index is greater than or equal to its corresponding bound
template <std::size_t T_dimensions = 0>
constexpr inline bool out_of_bounds(
	const fgl::random_access_range_integral auto& indexes,
	const fgl::random_access_range_integral auto& bounds)
noexcept
{
	const std::size_t dimensions{
		assert_dimensions<T_dimensions>(indexes, bounds)
	};
	for(const auto& [index, bound] : fgl::czip(dimensions, indexes, bounds))
	{
		if (index >= bound)
			return true;
	}
	return false;
}

// Helper function: throws `std::out_of_range` if any index is out of bounds
template <std::size_t T_dimensions = 0>
constexpr inline void check_index_bounds(
	const fgl::random_access_range_integral auto& indexes,
	const fgl::random_access_range_integral auto& bounds)
{
	if (out_of_bounds<T_dimensions>(indexes, bounds))
		throw std::out_of_range("dimensional indexes exceed bounds.");
}

} // namespace linear_matrix_utilities
} // namespace fgl

#endif // FGL_TYPES_DIMENSIONAL_ALIAS_UTILITIES_HPP_INCLUDED
