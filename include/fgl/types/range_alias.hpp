#pragma once
#ifndef FGL_TYPES_RANGE_ALIAS_HPP_INCLUDED
#define FGL_TYPES_RANGE_ALIAS_HPP_INCLUDED

/// QUICK-START GUIDE
/*
    const fgl::range_alias my_range(begin_iter, end_sentinel);
	// or
	const auto my_range{ make_range(raw_ptr, length) };

	for (const auto& v : my_range) ...

	fgl::range_alias can be used to easily create modern and safe C++ ranges
	from raw pointers, or other iterators. The helper function make_range()
	makes it easy to create a range from a C-style pointer and length.
*/
/// EXAMPLE PROGRAM
/*
	#include <cstddef> // size_t
	#include <iostream> // cout
	#include <ranges> // ranges::size
	#include <memory> // unique_ptr

	#include <fgl/types/range_alias.hpp>

	int main()
	{
		// heap allocate an array of 10 ints. unique_ptr so it'll be deleted.
		constexpr std::size_t length{ 10 };
		const std::unique_ptr<int[]> memory_owner(new int[length]);
		int* const ptr{ memory_owner.get() }; // raw ptr for readability

		// can be used to make a safe and modern interface
		const fgl::range_alias range1(ptr, ptr+length);

		const auto range2{ fgl::make_range(ptr, length) }; // helper factory

		std::cout // can use in sized algorithms
			<< "size of array: "
			<< std::ranges::size(range1)
			<< std::endl;

		for (int i{ 0 }; auto& value : range1)
			value = --i; // fill memory range

		for (const int i : range2)
			std::cout << i << ' '; // print memory range
	}
*/
/// EXAMPLE OUTPUT
/*
size of array: 10
-1 -2 -3 -4 -5 -6 -7 -8 -9 -10
*/

#include <iterator> // input_or_output_iterator, sentinel_for
#include <ranges> // range
#include <utility> // move
#include <type_traits> // is_pointer_v, is_same_v

namespace fgl {

// Range compatible wrapper for a T_begin iterator and T_end sentinel.
template
<
	std::input_or_output_iterator T_begin,
	std::sentinel_for<T_begin> T_end
>
struct range_alias
{
	T_begin m_begin;
	T_end m_end;

	public:
	constexpr range_alias() = delete;
	constexpr range_alias(const range_alias&) = delete; // why not copy?

	[[nodiscard]] explicit constexpr
	range_alias(const T_begin& begin, const T_end& end)
	: m_begin(begin), m_end(end)
	{ static_assert(std::ranges::range<range_alias<T_begin, T_end>>); }

	[[nodiscard]] explicit constexpr
	range_alias(T_begin&& begin, T_end&& end)
	: m_begin(std::move(begin)), m_end(std::move(end))
	{ static_assert(std::ranges::range<range_alias<T_begin, T_end>>); }

	[[nodiscard]] constexpr T_begin begin() const noexcept
	{ return m_begin; }

	[[nodiscard]] constexpr T_end end() const noexcept
	{ return m_end; }

	[[nodiscard]] constexpr const T_begin cbegin() const noexcept
	{ return m_begin; }

	[[nodiscard]] constexpr const T_end cend() const noexcept
	{ return m_end; }
};

// Constructs a range_alias from a (non-void) pointer and element count.
template <typename T>
requires std::is_pointer_v<T> && (std::is_same_v<T, void*> == false)
[[nodiscard]] constexpr auto make_range(
	const T pointer,
	const std::size_t elements) noexcept
{
	return range_alias(pointer, pointer + elements);
}

}// namespace fgl

#endif // FGL_TYPES_RANGE_ALIAS_HPP_INCLUDED
