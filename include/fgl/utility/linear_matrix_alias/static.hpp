#pragma once
#ifndef FGL_UTILITY_LINEAR_MATRIX_ALIAS_STATIC_HPP_INCLUDED
#define FGL_UTILITY_LINEAR_MATRIX_ALIAS_STATIC_HPP_INCLUDED
#include "../../environment/libfgl_compatibility_check.hpp"

/// QUICK-START GUIDE
/*
auto alias{ fgl::make_matrix_alias<x,y,z>(arg) }; // row by default
//auto alias{ fgl::make_matrix_alias<fgl::major::row, x,y,z>(arg) };
//auto alias{ fgl::make_matrix_alias<fgl::major::column, x,y,z>(arg) };

// arg can be either a begin iter or range. bounds-checked if range, size
// must equal product of bounds

int counter{};
for (std::size_t i{}; i < x; ++i)
for (std::size_t j{}; j < y; ++j)
for (std::size_t l{}; l < z; ++l)
{
	alias.at({i,j,l}) = counter; // bounds-checked, may throw
	//alias[{i,j,l}] = counter; // not bounds-checked
	++counter;
}

alias.set_iterator(arg); // only bounds-checked if arg is a range

*/
/// EXAMPLE PROGRAM
/*
#include <cstddef> // size_t
#include <iostream> // cout, endl
#include <array>

#include <fgl/utility/linear_matrix_alias/static.hpp>

int main()
{
	constexpr std::size_t x{3}, y{3}, z{3};
	std::array<short, x*y*z> buffer;
	for (short i{}; auto& v : buffer)
		v = i++; // fill buffer

	using enum fgl::major;
	auto alias_row{ fgl::make_matrix_alias<row, x,y,z>(buffer) };
	auto alias_column{ fgl::make_matrix_alias<column, x,y,z>(buffer) };

	/// equivalent to:
	// using iter_t = decltype(buffer.begin());
	// fgl::static_linear_matrix_alias<iter_t, row, x,y,z> alias_row(buffer);
	// fgl::static_linear_matrix_alias<iter_t, column, x,y,z> alias_column(buffer);

	std::cout << "row major: ";
	for (std::size_t i{}; i < x; ++i)
	for (std::size_t j{}; j < y; ++j)
	for (std::size_t l{}; l < z; ++l)
	{
		std::cout << alias_row[{i, j, l}] << ", ";
	}

	std::cout << "\ncolumn major: ";
	for (std::size_t i{}; i < x; ++i)
	for (std::size_t j{}; j < y; ++j)
	for (std::size_t l{}; l < z; ++l)
	{
		std::cout << alias_column[{i, j, l}] << ", ";
	}
	std::cout << std::endl;
}
*/
/// EXAMPLE OUTPUT
/*
row major: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,

column major: 0, 9, 18, 3, 12, 21, 6, 15, 24, 1, 10, 19, 4, 13, 22, 7, 16, 25, 2, 11, 20, 5, 14, 23, 8, 17, 26,
*/

#include <cstddef> // size_t
#include <stdexcept> // invalid_argument, out_of_range
#include <iterator> // contiguous_iterator
#include <concepts>
#include <ranges>

#include "./utilities.hpp"

namespace fgl {

template <major T_major, std::size_t ... T_bounds>
struct static_linear_matrix_info
{
	static constexpr std::size_t dimensions{ sizeof...(T_bounds) };
	using array_t = std::array<std::size_t, dimensions>;
	static constexpr array_t bounds{ T_bounds... };
	static constexpr array_t offsets{
		fgl::linear_matrix_utilities::make_offsets<dimensions, T_major>(bounds)
	};

	[[nodiscard]] static constexpr bool out_of_bounds(const array_t& indexes)
	{
		return fgl::linear_matrix_utilities::out_of_bounds
			<dimensions>(indexes, bounds);
	}

	static constexpr void check_bounds(const array_t& indexes)
	{
		fgl::linear_matrix_utilities::check_index_bounds
			<dimensions>(indexes, bounds);
	}

	// converts multi-dimensional indexes to the linear index
	template <std::integral T_out = std::ptrdiff_t>
	[[nodiscard]] static constexpr
	T_out convert_indexes(const array_t& indexes) noexcept
	{
		return fgl::linear_matrix_utilities::matrix_to_linear
			<dimensions, T_out>(indexes, offsets);
	}
};

/*A multi-dimensional alias to a contiguous range with template major,
dimensions, and bounds, which enables optimizations and compile-time
calculation of offsets.*/
template
<
	std::contiguous_iterator T_iter,
	major T_major,
	std::size_t ... T_bounds
>
class static_linear_matrix_alias
{
	T_iter m_iter;

	using static_info = fgl::static_linear_matrix_info<T_major, T_bounds...>;

	static constexpr void check_range_bounds(
		const std::ranges::contiguous_range auto& range)
	{
		constexpr std::size_t alias_len{ // TODO C++23 make static
			std::reduce(
				static_info::bounds.cbegin(),
				static_info::bounds.cend(),
				std::size_t{ 1 },
				std::multiplies<std::size_t>()
			)
		};
		if (alias_len != std::ranges::size(range)) throw std::invalid_argument(
			"static_linear_matrix_alias() range size must equal"
			" the product of the dimensional bounds"
		);
	}

public:
	using array_t = static_info::array_t;
	using difference_type = std::iter_difference_t<T_iter>;

	[[nodiscard]] explicit constexpr
	static_linear_matrix_alias(T_iter iter) noexcept
	: m_iter(iter)
	{}

	[[nodiscard]] explicit constexpr
	static_linear_matrix_alias(std::ranges::contiguous_range auto& range)
	: static_linear_matrix_alias(std::ranges::begin(range))
	{ check_range_bounds(range); }

	[[nodiscard]] static constexpr std::size_t dimensions() noexcept
	{ return static_info::dimensions; }

	[[nodiscard]] static constexpr const array_t& bounds() noexcept
	{ return static_info::bounds; }

	[[nodiscard]] static constexpr const array_t& offsets() noexcept
	{ return static_info::offsets; }

	[[nodiscard]] static constexpr major get_major() noexcept
	{ return T_major; }

	[[nodiscard]] static constexpr bool is_row_major() noexcept
	{ return T_major == major::row; }

	[[nodiscard]] static constexpr bool is_column_major() noexcept
	{ return T_major == major::column; }

	[[nodiscard]] constexpr auto iterator() const
	{ return m_iter; }

	constexpr void set_iterator(T_iter iter) noexcept
	{ m_iter = iter; }

	template <std::ranges::contiguous_range T_range>
	requires std::same_as<std::ranges::iterator_t<T_range>, T_iter>
	constexpr void set_iterator(T_range& range)
	{
		check_range_bounds(range);
		set_iterator(std::ranges::begin(range));
	}

	// converts multi-dimensional indexes to the linear index
	[[nodiscard]] static constexpr
	difference_type convert_indexes(const array_t& indexes) noexcept
	{ return static_info::template convert_indexes<difference_type>(indexes); }

	// Unsafe: unchecked index. Example: 3D index via `obj[{x,y,z}]`
	[[nodiscard]] constexpr auto& operator[](const array_t& indexes)
	{ return m_iter[static_info::convert_indexes(indexes)]; }

	// Unsafe: unchecked index. Example: 3D index via `obj[{x,y,z}]`
	[[nodiscard]] constexpr
	const auto& operator[](const array_t& indexes) const
	{ return m_iter[static_info::convert_indexes(indexes)]; }

	// Bounds-checked index. Example: 3D index via `obj.at({x,y,z})`
	[[nodiscard]] constexpr auto& at(const array_t& indexes)
	{
		static_info::check_bounds(indexes);
		return operator[](indexes);
	}

	// const bounds-checked index. Example: 3D index via `obj.at({x,y,z})`
	[[nodiscard]] constexpr const auto& at(const array_t& indexes) const
	{
		static_info::check_bounds(indexes);
		return operator[](indexes);
	}
};

/// Helper factories

// constructs a row major static dimensional alias from an iterator
template <std::convertible_to<std::size_t> auto ... T_bounds>
[[nodiscard]] constexpr
auto make_matrix_alias(std::contiguous_iterator auto iter) noexcept
{
	return static_linear_matrix_alias
		<
			decltype(iter),
			major::row, // detault
			static_cast<std::size_t>(T_bounds)...
		>(iter);
}

// constructs a static dimensional alias from an iterator
template <major T_major, std::convertible_to<std::size_t> auto ... T_bounds>
[[nodiscard]] constexpr
auto make_matrix_alias(std::contiguous_iterator auto iter) noexcept
{
	return static_linear_matrix_alias
		<
			decltype(iter),
			T_major,
			static_cast<std::size_t>(T_bounds)...
		>(iter);
}

// constructs a row major static dimensional alias from a bounds-checked range
template <std::convertible_to<std::size_t> auto ... T_bounds>
[[nodiscard]] constexpr
auto make_matrix_alias(std::ranges::contiguous_range auto& range)
{
	return static_linear_matrix_alias
		<
			std::ranges::iterator_t<decltype(range)>,
			major::row, // default
			static_cast<std::size_t>(T_bounds)...
		>(range);
}

// constructs a row major static dimensional alias from a bounds-checked range
template <major T_major, std::convertible_to<std::size_t> auto ... T_bounds>
[[nodiscard]] constexpr
auto make_matrix_alias(std::ranges::contiguous_range auto& range)
{
	return static_linear_matrix_alias
		<
			std::ranges::iterator_t<decltype(range)>,
			T_major,
			static_cast<std::size_t>(T_bounds)...
		>(range);
}


}// namespace fgl

#endif // FGL_UTILITY_LINEAR_MATRIX_ALIAS_STATIC_HPP_INCLUDED
