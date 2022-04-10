#pragma once
#ifndef FGL_UTILITY_LINEAR_MATRIX_ALIAS_DYNAMIC_HPP_INCLUDED
#define FGL_UTILITY_LINEAR_MATRIX_ALIAS_DYNAMIC_HPP_INCLUDED
#include "../../environment/libfgl_compatibility_check.hpp"

/// QUICK-START GUIDE
/*
using enum fgl::major;
auto alias{ fgl::make_matrix_alias(arg, x, y, z) }; // row by default
//auto alias{ fgl::make_matrix_alias(arg, row, x, y, z) };
//auto alias{ fgl::make_matrix_alias(arg, column, x, y, z) };

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

alias.set_major(major);
alias.switch_major(); // toggles between majors

alias.set_bounds({x,y,z}); // not bounds-checked

// simultanuous update
alias.update(arg, {x,y,z}); // doesnt change major
alias.update(arg, {x,y,z}, major);
*/
/// EXAMPLE PROGRAM
/*
#include <cstddef> // size_t
#include <iostream> // cout, endl
#include <vector>

#include <fgl/utility/linear_matrix_alias/dynamic.hpp>

int main()
{
	std::size_t x{ 3 }, y{ 3 }, z{ 3 };
	std::vector<short> buffer;
	buffer.resize(x*y*z);
	for (short i{}; auto& v : buffer)
		v = i++; // fill buffer

	using enum fgl::major;
	auto alias{ fgl::make_matrix_alias(buffer, row, x,y,z) };

	std::cout << "row major " << x << 'x' << y << 'x' << z << ":\n";
	for (std::size_t i{}; i < x; ++i)
	for (std::size_t j{}; j < y; ++j)
	for (std::size_t l{}; l < z; ++l)
	{
		std::cout << alias[{i, j, l}] << ", ";
	}

	++x, ++y, ++z;
	buffer.resize(x*y*z);
	for (short i{}; auto& v : buffer)
		v = i++; // fill buffer

	alias.update(buffer, {x,y,z}, column);

	std::cout << "\n\ncolumn major " << x << 'x' << y << 'x' << z << ":\n";
	for (std::size_t i{}; i < x; ++i)
	for (std::size_t j{}; j < y; ++j)
	for (std::size_t l{}; l < z; ++l)
	{
		std::cout << alias[{i, j, l}] << ", ";
	}
	std::cout << std::endl;
}
*/
/// EXAMPLE OUTPUT
/*
row major 3x3x3:
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,

column major 4x4x4:
0, 16, 32, 48, 4, 20, 36, 52, 8, 24, 40, 56, 12, 28, 44, 60, 1, 17, 33, 49, 5, 21, 37, 53, 9, 25, 41, 57, 13, 29, 45, 61, 2, 18, 34, 50, 6, 22, 38, 54, 10, 26, 42, 58, 14, 30, 46, 62, 3, 19, 35, 51, 7, 23, 39, 55, 11, 27, 43, 59, 15, 31, 47, 63,
*/

#include <cstddef> // size_t
#include <stdexcept> // invalid_argument, out_of_range
#include <iterator> // contiguous_iterator iter_difference_t
#include <concepts>
#include <ranges>

#include "../../types/range_constraints.hpp"
#include "./utilities.hpp"

namespace fgl {

/*A multi-dimensional alias to a contiguous range with dynamic bounds*/
template <std::contiguous_iterator T_iter, std::size_t T_dimensions>
class dynamic_linear_matrix_alias
{
public:
	using array_t = std::array<std::size_t, T_dimensions>;
	using difference_type = std::iter_difference_t<T_iter>;

	dynamic_linear_matrix_alias() = delete;
	dynamic_linear_matrix_alias(const dynamic_linear_matrix_alias&) = delete;
	dynamic_linear_matrix_alias(dynamic_linear_matrix_alias&&) = delete;
	void operator=(const dynamic_linear_matrix_alias&) = delete;

	[[nodiscard]] explicit constexpr dynamic_linear_matrix_alias(
		T_iter iter,
		const array_t& bounds,
		const major major = major::row)
	noexcept
	:
		m_major(major),
		m_bounds(bounds),
		m_offsets{
			m_major == major::row
				? fgl::linear_matrix_utilities::make_offsets
					<T_dimensions, major::row>(m_bounds)
				: fgl::linear_matrix_utilities::make_offsets
					<T_dimensions, major::column>(m_bounds)
		},
		m_iter(iter)
	{}

	[[nodiscard]] explicit constexpr dynamic_linear_matrix_alias(
		std::ranges::contiguous_range auto& range,
		const array_t& bounds,
		const major major = major::row)
	: dynamic_linear_matrix_alias(std::ranges::begin(range), bounds, major)
	{ check_range_bounds(range, bounds); }

	[[nodiscard]] constexpr std::size_t dimensions() const noexcept
	{ return T_dimensions; }

	[[nodiscard]] constexpr const array_t& bounds() const noexcept
	{ return m_bounds; }

	constexpr void set_bounds(const array_t& bounds) noexcept
	{
		m_bounds = bounds;
		recalculate_offsets();
	}

	[[nodiscard]] constexpr const array_t& offsets() const noexcept
	{ return m_offsets; }

	[[nodiscard]] constexpr major get_major() const noexcept
	{ return m_major; }

	[[nodiscard]] constexpr bool is_row_major() const noexcept
	{ return get_major() == fgl::major::row; }

	[[nodiscard]] constexpr bool is_column_major() const noexcept
	{ return get_major() == fgl::major::column; }

	constexpr void switch_major() noexcept
	{
		m_major = is_row_major() ? major::column : major::row;
		recalculate_offsets();
	}

	constexpr void set_major(const fgl::major new_major) noexcept
	{
		if (m_major != new_major)
		{
			m_major = new_major;
			recalculate_offsets();
		}
	}

	[[nodiscard]] constexpr auto iterator() const noexcept
	{ return m_iter; }

	constexpr void set_iterator(T_iter iter) noexcept
	{ m_iter = iter; }

	template <std::ranges::contiguous_range T_range>
	requires std::same_as<std::ranges::iterator_t<T_range>, T_iter>
	constexpr void set_iterator(T_range& range)
	{
		check_range_bounds(range, m_bounds);
		set_iterator(std::ranges::begin(range));
	}

	constexpr void update(T_iter iter, const array_t& bounds) noexcept
	{
		set_iterator(iter);
		set_bounds(bounds);
	}

	constexpr void update(
		T_iter iter,
		const array_t& bounds,
		const fgl::major new_major) noexcept
	{
		set_major(new_major);
		update(iter, bounds);
	}

	template <std::ranges::contiguous_range T_range>
	requires std::same_as<std::ranges::iterator_t<T_range>, T_iter>
	constexpr void update(T_range& range, const array_t& bounds)
	{
		check_range_bounds(range, bounds);
		update(std::ranges::begin(range), bounds);
	}

	template <std::ranges::contiguous_range T_range>
	requires std::same_as<std::ranges::iterator_t<T_range>, T_iter>
	constexpr void update(
		T_range& range,
		const array_t& bounds,
		const fgl::major new_major)
	{
		check_range_bounds(range, bounds);
		set_major(new_major);
		update(std::ranges::begin(range), bounds);
	}

	// converts multi-dimensional indexes to the linear index
	[[nodiscard]] constexpr
	difference_type convert_indexes(const array_t& indexes) const noexcept
	{
		using fgl::linear_matrix_utilities::matrix_to_linear;
		return matrix_to_linear<T_dimensions, difference_type>(
			indexes, m_offsets
		);
	}

	// Unsafe: unchecked index. Example: 3D index via `obj[{x,y,z}]`
	[[nodiscard]] constexpr auto& operator[](const array_t& indexes)
	{ return m_iter[convert_indexes(indexes)]; }

	// Unsafe: unchecked index. Example: 3D index via `obj[{x,y,z}]`
	[[nodiscard]] constexpr
	const auto& operator[](const array_t& indexes) const
	{ return m_iter[convert_indexes(indexes)]; }

	// Bounds-checked index. Example: 3D index via `obj.at({x,y,z})`
	[[nodiscard]] constexpr auto& at(const array_t& indexes)
	{
		fgl::linear_matrix_utilities::check_index_bounds
			<T_dimensions>(indexes, m_bounds);
		return operator[](indexes);
	}

	// const bounds-checked index. Example: 3D index via `obj.at({x,y,z})`
	[[nodiscard]] constexpr const auto& at(const array_t& indexes) const
	{
		fgl::linear_matrix_utilities::check_index_bounds
			<T_dimensions>(indexes, m_bounds);
		return operator[](indexes);
	}

private:
	constexpr void recalculate_offsets() noexcept
	{
		if (is_row_major())
		{
			fgl::linear_matrix_utilities::calculate_offsets
				<T_dimensions, major::row>(m_bounds, m_offsets);
		}
		else
		{
			fgl::linear_matrix_utilities::calculate_offsets
				<T_dimensions, major::column>(m_bounds, m_offsets);
		}
	}

	static constexpr void check_range_bounds(
		const std::ranges::contiguous_range auto& range,
		const array_t& bounds)
	{
		const std::size_t alias_len{
			std::reduce(
				bounds.cbegin(),
				bounds.cend(),
				std::size_t{ 1 },
				std::multiplies<std::size_t>()
			)
		};
		if (alias_len != std::ranges::size(range)) throw std::invalid_argument(
			"dynamic_linear_matrix_alias() range size must equal"
			" the product of the dimensional bounds"
		);
	}

	fgl::major m_major;
	array_t m_bounds;
	array_t m_offsets;
	T_iter m_iter;
};

/// Deduction guide
template <std::ranges::contiguous_range T_range, std::size_t T_dimensions>
dynamic_linear_matrix_alias(
	T_range&,
	const std::array<std::size_t, T_dimensions>&,
	const fgl::major
)->dynamic_linear_matrix_alias<std::ranges::iterator_t<T_range>, T_dimensions>;


/// Helper factories

// constructs a row major dynamic dimensional alias from an iterator
[[nodiscard]] constexpr inline auto make_matrix_alias(
	std::contiguous_iterator auto iter,
	const std::convertible_to<std::size_t> auto ... bounds)
noexcept
{
	return dynamic_linear_matrix_alias(
		iter, std::array{ static_cast<std::size_t>(bounds)... }, true // row
	);
}

// constructs a dynamic dimensional alias from an iterator
[[nodiscard]] constexpr inline auto make_matrix_alias(
	std::contiguous_iterator auto iter,
	const major major,
	const std::convertible_to<std::size_t> auto ... bounds)
noexcept
{
	return dynamic_linear_matrix_alias(
		iter, std::array{ static_cast<std::size_t>(bounds)... }, major
	);
}

// constructs a row major dynamic dimensional alias from a bounds-checked range
[[nodiscard]] constexpr inline auto make_matrix_alias(
	std::ranges::contiguous_range auto& range,
	const std::convertible_to<std::size_t> auto ... bounds)
{
	return dynamic_linear_matrix_alias(
		range, std::array{ static_cast<std::size_t>(bounds)... }, true // row
	);
}

// constructs a dynamic dimensional alias from a bounds-checked range
[[nodiscard]] constexpr inline auto make_matrix_alias(
	std::ranges::contiguous_range auto& range,
	const major major,
	const std::convertible_to<std::size_t> auto ... bounds)
{
	return dynamic_linear_matrix_alias(
		range, std::array{ static_cast<std::size_t>(bounds)... }, major
	);
}

}// namespace fgl

#endif // FGL_UTILITY_LINEAR_MATRIX_ALIAS_DYNAMIC_HPP_INCLUDED
