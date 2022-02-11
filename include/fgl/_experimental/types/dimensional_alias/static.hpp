#pragma once
#ifndef FGL_TYPES_DIMENSIONAL_ALIAS_STATIC_HPP_INCLUDED
#define FGL_TYPES_DIMENSIONAL_ALIAS_STATIC_HPP_INCLUDED

/// QUICK-START GUIDE
/*
*/
/// EXAMPLE PROGRAM
/*
TODO
*/
/// EXAMPLE OUTPUT
/*
Row major:

        Multi-dimensional:
         0  1  2  3  4
         5  6  7  8  9
        10 11 12 13 14
        15 16 17 18 19
        20 21 22 23 24

        Linear:
        0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24


 Column major:

        Multi-dimensional:
         0  5 10 15 20
         1  6 11 16 21
         2  7 12 17 22
         3  8 13 18 23
         4  9 14 19 24

        Linear:
        0 5 10 15 20 1 6 11 16 21 2 7 12 17 22 3 8 13 18 23 4 9 14 19 24
*/

#include <cstddef> // size_t
#include <stdexcept> // invalid_argument, out_of_range
#include <iterator> // contiguous_iterator
#include <concepts>
#include <ranges>

//#include "../../types/ctypes.hpp" // carray

#include "utilities.hpp"

namespace fgl {

/*A static structure which contains the dimensions, bounds, and index offsets
for a static dimensional alias.*/
template <bool T_row_major, std::size_t ... T_bounds>
struct static_dimensional_alias_info
{
	static constexpr std::size_t dimensions{ sizeof...(T_bounds) };
	//using carray_t = fgl::carray<std::size_t, dimensions>;
	using array_t = std::array<std::size_t, dimensions>;
	static constexpr array_t bounds{ T_bounds... };
	static constexpr array_t offsets{
		T_row_major
			? fgl::dimensional_utilities::make_offsets_major_row
				<dimensions>(bounds)
			: fgl::dimensional_utilities::make_offsets_major_column
				<dimensions>(bounds)
	};

	[[nodiscard]] constexpr bool out_of_bounds(const array_t& indexes) const
	{
		return fgl::dimensional_utilities::out_of_bounds
			<dimensions>(indexes, bounds);
	}

	constexpr void check_bounds(const array_t& indexes) const
	{
		fgl::dimensional_utilities::check_index_bounds
			<dimensions>(indexes, bounds);
	}

	// converts multi-dimensional indexes to the linear index
	[[nodiscard]] constexpr
	std::size_t convert_indexes(const array_t& indexes) const
	{
		return fgl::dimensional_utilities::dimensional_to_linear
			<dimensions>(indexes, offsets);
	}
};

/*A multi-dimensional alias to a contiguous range with static dimensions,
which enables optimizations and compile-time calculation of alias info.*/
template
<
	std::contiguous_iterator T_iter,
	bool T_row_major = true,
	std::size_t ... T_bounds
>
class static_dimensional_alias
{
	const T_iter m_iter;

public:
	static constexpr static_dimensional_alias_info<T_row_major, T_bounds...>
		info{};

	//using carray_t = decltype(info)::carray_t;
	using array_t = decltype(info)::array_t;

	[[nodiscard]] explicit constexpr
	static_dimensional_alias(T_iter iter)
	: m_iter(iter)
	{}

	[[nodiscard]] explicit constexpr
	static_dimensional_alias(std::ranges::contiguous_range auto& range)
	: static_dimensional_alias(std::ranges::begin(range))
	{
		std::size_t alias_len{ 1 };
		for (const auto bound : info.bounds)
			alias_len *= bound;

		if (alias_len != std::ranges::size(range)) throw std::invalid_argument(
			"static_dimensional_alias() range size must equal"
			" the product of the dimensional bounds"
		);
	}

	// returns the underlying iterator
	[[nodiscard]] constexpr auto begin() const
	{ return m_iter; }

	// Unsafe: unchecked index. Example: 3D index via `obj[{x,y,z}]`
	[[nodiscard]] constexpr auto& operator[](const array_t& indexes)
	{ return m_iter[info.convert_indexes(indexes)]; }

	// Unsafe: unchecked index. Example: 3D index via `obj[{x,y,z}]`
	[[nodiscard]] constexpr
	const auto& operator[](const array_t& indexes) const
	{ return m_iter[info.convert_indexes(indexes)]; }

	// Bounds-checked index. Example: 3D index via `obj.at({x,y,z})`
	[[nodiscard]] constexpr auto& at(const array_t& indexes)
	{
		info.check_bounds(indexes);
		return operator[](indexes);
	}

	// const bounds-checked index. Example: 3D index via `obj.at({x,y,z})`
	[[nodiscard]] constexpr const auto& at(const array_t& indexes) const
	{
		info.check_bounds(indexes);
		return operator[](indexes);
	}
};

// helper factory function with a concise interface
template <std::size_t ... T_bounds, bool T_row_major = true>
[[nodiscard]] constexpr
auto make_dimensional_alias(std::contiguous_iterator auto iter)
{
	return static_dimensional_alias
		<
			decltype(iter),
			T_row_major,
			T_bounds...
		>(iter);
}

// helper factory function with a concise interface
template <std::size_t ... T_bounds, bool T_row_major = true>
[[nodiscard]] constexpr
auto make_dimensional_alias(std::ranges::contiguous_range auto& range)
{
	return static_dimensional_alias
		<
			std::ranges::iterator_t<decltype(range)>,
			T_row_major,
			T_bounds...
		>(range);
}

}// namespace fgl

#endif // FGL_TYPES_DIMENSIONAL_ALIAS_STATIC_HPP_INCLUDED
