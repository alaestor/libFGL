#pragma once
#ifndef FGL_UTILITY_MATRIX_HPP_INCLUDED
#define FGL_UTILITY_MATRIX_HPP_INCLUDED
#include "../environment/libfgl_compatibility_check.hpp"

/// QUICKSTART GUIDE
/*
fgl::matrix<int, x,y,z,w> matrix;
matrix[{0,0,0,0}] = 0;
matrix.at({0,0,0,0}) = 0;

matrix.linear_index(0) = 0;
matrix.linear_at(0) = 0;

std::array<int, x*y*z*w>& array{ matrix.linear_array() };
*/
/// EXAMPLE PROGRAM
/*
#include <cstddef> // size_t
#include <iostream> // cout, endl
#include <iomanip> // setw

#include <fgl/utility/matrix.hpp>

int main()
{
	constexpr std::size_t x{ 5 }, y{ 5 };
	fgl::matrix<short, x, y> matrix;

	for (short i{}; auto& v : matrix) // using std::array's begin/end
		v = i++; // fill matrix (row major)

	std::cout << "Matrix " << x << 'x' << y << ":\n";
	for (std::size_t i{}; i < x; ++i)
	{
		for (std::size_t j{}; j < y; ++j)
		{
			std::cout << std::setw(3) << matrix[{i, j}] << ", ";
		}
		std::cout << std::endl;
	}
}
*/
/// EXAMPLE PROGRAM OUTPUT
/*
Matrix 5x5:
  0,   1,   2,   3,   4,
  5,   6,   7,   8,   9,
 10,  11,  12,  13,  14,
 15,  16,  17,  18,  19,
 20,  21,  22,  23,  24,
 */

#include <cstddef> // size_t
#include <array>

#include "./linear_matrix_alias/utilities.hpp"
#include "./linear_matrix_alias/static.hpp" //static_linear_matrix_info

namespace fgl {

// linear iterators. multi-dimensional access is only available thru container.

// a row-major multi-dimensional array
template <typename T, std::size_t ... T_bounds>
class matrix : public std::array<T, (T_bounds * ...)>
{
	using static_info = fgl::static_linear_matrix_info
		<fgl::major::row, T_bounds...>;

	public:
	// array of size_t, one element corresponding to each dimension
	using dimensional_t = static_info::array_t;
	using base_t = std::array<T, (T_bounds * ...)>;

	[[nodiscard]] static constexpr std::size_t dimensions() noexcept
	{ return static_info::dimensions; }

	[[nodiscard]] static constexpr const dimensional_t& bounds() noexcept
	{ return static_info::bounds; }

	[[nodiscard]] static constexpr const dimensional_t& offsets() noexcept
	{ return static_info::offsets; }

	// converts multi-dimensional indexes to the linear index
	[[nodiscard]] static constexpr
	std::size_t convert_indexes(const dimensional_t& indexes) noexcept
	{ return static_cast<std::size_t>(static_info::convert_indexes(indexes)); }

	// base class downcasting wrapper
	[[nodiscard]] constexpr base_t& linear_array() noexcept
	{ return *static_cast<base_t*>(this); }

	// base class downcasting wrapper
	[[nodiscard]] constexpr const base_t& linear_array() const noexcept
	{ return *static_cast<const base_t*>(this); }

	constexpr matrix& operator=(const matrix&) = default;
	constexpr matrix& operator=(const base_t& rhs)
	{
		linear_array() = rhs;
		return *this;
	}

	constexpr auto operator[](base_t::size_type) = delete;
	constexpr auto operator[](base_t::size_type) const = delete;
	constexpr auto at(base_t::size_type) = delete;
	constexpr auto at(base_t::size_type) const = delete;

	[[nodiscard]] constexpr auto& linear_index(const std::size_t index)
	{ return linear_array()[index]; }

	[[nodiscard]] constexpr
	const auto& linear_index(const std::size_t index) const
	{ return linear_array()[index]; }

	[[nodiscard]] constexpr auto& linear_at(const std::size_t index)
	{ return linear_array().at(index); }

	[[nodiscard]] constexpr
	const auto& linear_at(const std::size_t index) const
	{ return linear_array().at(index); }

	// Unsafe: unchecked index. Example: 3D index via `obj[{x,y,z}]`
	[[nodiscard]] constexpr auto& operator[](const dimensional_t& indexes)
	{ return linear_array()[convert_indexes(indexes)]; }

	// Unsafe: unchecked index. Example: 3D index via `obj[{x,y,z}]`
	[[nodiscard]] constexpr
	const auto& operator[](const dimensional_t& indexes) const
	{ return linear_array()[convert_indexes(indexes)]; }

	// Bounds-checked index. Example: 3D index via `obj.at({x,y,z})`
	[[nodiscard]] constexpr auto& at(const dimensional_t& indexes)
	{
		static_info::check_bounds(indexes);
		return operator[](indexes);
	}

	// const bounds-checked index. Example: 3D index via `obj.at({x,y,z})`
	[[nodiscard]] constexpr const auto& at(const dimensional_t& indexes) const
	{
		static_info::check_bounds(indexes);
		return operator[](indexes);
	}

	/* for a T_major template version:
	[[nodiscard]] constexpr major major() const noexcept
	{ return T_major; }
	[[nodiscard]] constexpr bool is_row_major() const noexcept
	{ return T_major == major::row; }
	[[nodiscard]] constexpr bool is_column_major() const noexcept
	{ return T_major == major::column; }
	[[nodiscard]] constexpr reference back()
	{ return operator[]({T_bounds-1 ...}); }
	[[nodiscard]] constexpr const_reference back() const
	{ return operator[]({T_bounds-1 ...}); }
	need to override begin and end iterators
	*/
};

/*
template <typename T, std::size_t ... T_bounds>
[[nodiscard]] constexpr bool operator==(
	const fgl::matrix<T, T_bounds...>& lhs,
	const fgl::matrix<T, T_bounds...>& rhs)
{ return lhs.linear_array() == rhs.linear_array(); }

template <typename T, std::size_t ... T_bounds>
[[nodiscard]] constexpr auto operator<=>(
	const fgl::matrix<T, T_bounds...>& lhs,
	const fgl::matrix<T, T_bounds...>& rhs)
{ return lhs.linear_array() <=> rhs.linear_array(); }
//*/

template <typename T, std::size_t ... T_bounds>
[[nodiscard]] constexpr bool operator==(
	const std::array<T, (T_bounds * ...)>& lhs,
	const fgl::matrix<T, T_bounds...>& rhs)
{ return lhs == rhs.linear_array(); }

template <typename T, std::size_t ... T_bounds>
[[nodiscard]] constexpr auto operator<=>(
	const std::array<T, (T_bounds * ...)>& lhs,
	const fgl::matrix<T, T_bounds...>& rhs)
{ return lhs <=> rhs.linear_array(); }

} // namespace fgl

#endif // FGL_UTILITY_MATRIX_HPP_INCLUDED
