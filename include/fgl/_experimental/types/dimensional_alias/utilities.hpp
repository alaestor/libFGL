#pragma once
#ifndef FGL_TYPES_DIMENSIONAL_ALIAS_UTILITIES_HPP_INCLUDED
#define FGL_TYPES_DIMENSIONAL_ALIAS_UTILITIES_HPP_INCLUDED

#include <cstddef> // size_t
#include <stdexcept> // out_of_range
#include <concepts>
#include <ranges>
#include <numeric> // reduce
#include <functional> // multiplies

#include "../../../debug/constexpr_assert.hpp"
#include "../../../utility/zip.hpp" // czip
#include "../../../types/range_constraints.hpp"

namespace fgl::dimensional_utilities {

/*Asserts that the ranges are of equal size, and returns that size.
If T_dimensions is zero, asserts that the sizes of a and b are the same and
returns the size. If T_dimensions isn't zero, asserts the sizes of a and b are
T_dimensions, and returns T_dimensions.*/
template <std::size_t T_dimensions = 0>
[[nodiscard]] constexpr inline std::size_t assert_dimensions(
	const std::ranges::sized_range auto& a,
	const std::ranges::sized_range auto& b)
{
	if constexpr (T_dimensions == 0)
	{
		std::size_t size{ std::ranges::size(a) };
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

/*Converts dimensional indexes to a linear index. The std::ranges::size of
indexes and offsets must be the same. If T_dimensions is zero the size of the
range will be used, otherwise T_dimensions is asserted to be the range size.*/
template <std::size_t T_dimensions = 0>
[[nodiscard]] constexpr inline std::size_t dimensional_to_linear(
	const fgl::contiguous_range_convertible_to<std::size_t> auto& indexes,
	const fgl::contiguous_range_convertible_to<std::size_t> auto& offsets)
{
	const std::size_t dimensions{
		assert_dimensions<T_dimensions>(indexes, offsets)
	};
	// for each dim: index += index[dim#] * PI(bounds[0] .. bounds[dim#])
	std::size_t index{ 0 };
	for (const auto& [i, o] : fgl::czip(dimensions, indexes, offsets))
		index += static_cast<std::size_t>(i) * static_cast<std::size_t>(o);
	return index;
}

// for each dim: out_offsets[dim#] = PI(bounds[0] .. bounds[dim#])
template
<
	std::size_t T_dimensions = 0,
	fgl::contiguous_range_integral T_bounds,
	fgl::contiguous_range_assignable_from
		<std::ranges::range_value_t<T_bounds>> T_offsets
>
constexpr inline void calculate_offsets_major_column(
	const T_bounds& bounds,
	T_offsets& out_offsets)
{
	const std::size_t dimensions{
		assert_dimensions<T_dimensions>(bounds, out_offsets)
	};
	// TODO use fgl::enumerate?
	using out_t = std::ranges::range_value_t<T_bounds>;
	for (std::size_t i{ 0 }; i < dimensions; ++i)
	{
		out_offsets[i] = std::reduce(
			&bounds[0],
			&bounds[i],
			out_t{ 1 },
			std::multiplies<out_t>()
		);
	}
}

// helper function: constructs, fills, and then returns a std::array of offsets
template <std::size_t T_dimensions, typename T_out = std::size_t>
constexpr inline std::array<T_out, T_dimensions> make_offsets_major_column(
    const fgl::contiguous_range_integral auto& bounds)
{
	std::array<T_out, T_dimensions> out;
	calculate_offsets_major_column<T_dimensions>(bounds, out);
	return out;
}

// for each dim: i = dim#; out_offsets[--i] = PI(bounds[0] .. bounds[dim#])
template
<
	std::size_t T_dimensions = 0,
	fgl::contiguous_range_integral T_bounds,
	fgl::contiguous_range_assignable_from
		<std::ranges::range_value_t<T_bounds>> T_offsets
>
constexpr inline void calculate_offsets_major_row(
	const T_bounds& bounds,
	T_offsets& out_offsets)
{
    const std::size_t dimensions{
        assert_dimensions<T_dimensions>(bounds, out_offsets)
    };
    // TODO use fgl::enumerate with a reverse adapter?
	using out_t = std::ranges::range_value_t<T_bounds>;
    for (std::size_t r{ dimensions-1 }, i{ 0 }; i < dimensions; ++i, --r)
    {
        out_offsets[r] = std::reduce(
            &bounds[0],
            &bounds[i],
            out_t{ 1 },
            std::multiplies<out_t>()
        );
    }
}

// helper function: constructs, fills, and then returns a std::array of offsets
template <std::size_t T_dimensions, typename T_out = std::size_t>
constexpr inline std::array<T_out, T_dimensions> make_offsets_major_row(
    const fgl::contiguous_range_integral auto& bounds)
{
	std::array<T_out, T_dimensions> out;
	calculate_offsets_major_row<T_dimensions>(bounds, out);
	return out;
}

// returns true if any index is greater than or equal to its corresponding bound
template <std::size_t T_dimensions = 0>
constexpr inline bool out_of_bounds(
	const fgl::contiguous_range_integral auto& indexes,
	const fgl::contiguous_range_integral auto& bounds)
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

// throws if any index is greater than or equal to its corresponding bound
template <std::size_t T_dimensions = 0>
constexpr inline void check_index_bounds(
	const fgl::contiguous_range_integral auto& indexes,
	const fgl::contiguous_range_integral auto& bounds)
{
	if (out_of_bounds<T_dimensions>(indexes, bounds))
		throw std::out_of_range("dimensional indexes exceed bounds.");
}

} // namespace fgl::dimensional_utilities

#endif // FGL_TYPES_DIMENSIONAL_ALIAS_UTILITIES_HPP_INCLUDED
