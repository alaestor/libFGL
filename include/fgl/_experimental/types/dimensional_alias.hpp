#pragma once
#ifndef FGL_TYPES_DIMENSIONAL_ALIAS_HPP_INCLUDED
#define FGL_TYPES_DIMENSIONAL_ALIAS_HPP_INCLUDED

/// QUICK-START GUIDE
/*
*/
/// EXAMPLE PROGRAM
/*
#include <cstddef> // size_t
#include <iostream>
#include <array>

#include <fgl/types/dimensional_alias.hpp>

int main()
{
	constexpr std::size_t x{ 5 }, y{ 5 };

	std::array<int, x*y> mem{ 0 };
    dimensional_alias buffer(mem, {x, y});

	// Fill
    for (std::size_t i{ 0 }; i < x; ++i)
  		for (std::size_t j{ 0 }; j < y; ++j)
   			buffer.at({i,j}) = static_cast<int>(i+j); // bounds-checked

    // Print
    for (std::size_t i{ 0 }; i < x; ++i)
	{
    	for (std::size_t j{ 0 }; j < y; ++j)
		{ std::cout << buffer[{i,j}]; } // unchecked
		std::cout << std::endl;
	}
}
*/
/// EXAMPLE OUTPUT
/*
01234
12345
23456
34567
45678
*/

#include <cstddef> // size_t
#include <iterator>
#include <ranges>
#include <concepts>
#include <stdexcept> // invalid_argument, out_of_range

#include <../utility/zip.hpp>

namespace fgl {

// Multi-dimensionally indexable alias for a linear contiguous range
template <std::contiguous_iterator T_iter, std::size_t T_dimensions>
class dimensional_alias
{
	public:
	//constexpr static std::size_t dimensions{ T_dimensions };

	/* why not just take a std::array, you ask?
	I have no idea. Template decution hurts me. You can try it if you'd like.*/

	using bounds_t = std::size_t[T_dimensions];
	using indexes_t = std::array<std::size_t, T_dimensions>;

	// not bounds-checked.
	[[nodiscard]] explicit constexpr
	dimensional_alias(T_iter iter, const bounds_t& dimensional_bounds)
	:
		m_iter(iter),
		m_bounds{ std::to_array(dimensional_bounds) },
		m_offsets{ calculate_offets() }
	{}

	// bounds-checked
	[[nodiscard]] explicit constexpr
	dimensional_alias(
		std::ranges::contiguous_range auto& range,
		const bounds_t& dimensional_bounds)
	: dimensional_alias(std::ranges::begin(range), dimensional_bounds)
	{
		std::size_t alias_len{ 1 };
		for (const auto bound : m_bounds)
			alias_len *= bound;

		if (alias_len != std::ranges::size(range)) throw std::invalid_argument(
			"dimensional_alias() product of dimensions must equal range size"
		);
	}

	// Unsafe: unchecked index. Call example: 3D index via `obj[{x,y,z}]`
	[[nodiscard]] constexpr auto& operator[](const indexes_t& indexes)
	{ return m_iter[convert_dimensional_indexes(indexes)]; }

	// Unsafe: unchecked index. Call example: 3D index via `obj[{x,y,z}]`
	[[nodiscard]] constexpr
	const auto& operator[](const indexes_t& indexes) const
	{ return m_iter[convert_dimensional_indexes(indexes)]; }

	// Bounds-checked index. Call example: 3D index via `obj.at({x,y,z})`
	[[nodiscard]] constexpr auto& at(const indexes_t& indexes)
	{
		check_bounds(indexes);
		return operator[](indexes);
	}

	// const bounds-checked index. Call example: 3D index via `obj.at({x,y,z})`
	[[nodiscard]] constexpr const auto& at(const indexes_t& indexes) const
	{
		check_bounds(indexes);
		return operator[](indexes);
	}

	private:
	T_iter m_iter;
	const indexes_t m_bounds;
	const indexes_t m_offsets;

	constexpr indexes_t calculate_offets()
	{
		indexes_t offset;
		for (std::size_t i{ 0 }; i < T_dimensions; ++i)
		{
			offset[i] = 1;
			for (std::size_t j{ 0 }; j < i; ++j)
			{
				offset[i] *= m_bounds[j];
			}
		}
		return offset;
	}

	constexpr void check_bounds(const indexes_t& indexes) const
	{
		for(const auto& [index, bound]
			: fgl::czip(T_dimensions, indexes, m_bounds))
		{
			if (index >= bound) throw std::out_of_range(
				"dimensional_alias indexes exceed dimensional bounds."
			);
		}
	}

	// converts dimensional indexes into the true linear index
	[[nodiscard]] constexpr
	std::size_t convert_dimensional_indexes(const indexes_t& indexes)
	const noexcept
	{
		// for each dim, index += index[#dim] * PI(bounds[0] .. bounds[#dim])
		std::size_t linear_index{ 0 };
		for (const auto& [i, o] : fgl::czip(T_dimensions, indexes, m_offsets))
			linear_index += i * o;
		return linear_index;
	}
};

// template deduction guide for contiguous range ctor
template <std::ranges::contiguous_range T_range, std::size_t T_dimensions>
dimensional_alias(T_range&, const std::size_t(&)[T_dimensions])
-> dimensional_alias<std::ranges::iterator_t<T_range>, T_dimensions>;

}// namespace fgl

#endif // FGL_TYPES_DIMENSIONAL_ALIAS_HPP_INCLUDED
