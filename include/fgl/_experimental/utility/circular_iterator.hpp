#pragma once
#ifndef FGL_UTILITY_CIRCULAR_ITERATOR_HPP_INCLUDED
#define FGL_UTILITY_CIRCULAR_ITERATOR_HPP_INCLUDED
#include "../environment/libfgl_compatibility_check.hpp"

/// QUICKSTART
/*
std::array<int, 10> r;

fgl::circular_iterator iter(r.begin(), r.end());
fgl::circular_iterator iter(r.begin(), r.end(), r.begin() + 3);
fgl::circular_iterator iter(r);
fgl::circular_iterator iter(r, r.begin() + 3);

*iter;
&*(iter -= 1) == &r.back();
&*(iter += 1) == &r.front();
iter[r.size() * 3]; // OK

auto x = iter;
auto y = iter + 2;
x - y == 8
y - x == 2
*/
/// EXAMPLE PROGRAM
/*
#include <cassert>
#include <array>
#include <iostream>
#include <fgl/utility/circular_iterator.hpp>

int main()
{
	std::array<int, 5> arr{};
	auto alias{ fgl::make_circular_range_alias(arr) };
	assert(
		alias.begin()
		== fgl::circular_iterator(arr.begin(), arr.end(), arr.begin())
	);

	for (int counter{}; auto& elem : alias)
	{
		if (counter == 100)
			break;

		elem = counter++;
	}

	for (const int elem : arr)
		std::cout << elem << ", ";
	std::cout << std::endl;
}
*/
/// EXAMPLE PROGRAM OUTPUT
/*
95, 96, 97, 98, 99,
*/
/// EXPLANATION
/*

Circular iterator is a random access iterator which models a circular range
via wrap-around behavior. By definiton, this iterator can never go out of
bounds (but it can still be invalidated). A circular iterator can be indexed
beyond the size of the underlying range in both positive and negative
directions to traverse forwards or backwards around the circle.

For example, given a range that has `N` elements, if a circular iterator to
the `0`th element is created and the iterator is incremented `N` times, its
state will be identical to when it was initialized; aliasing the `0`th
element. If the iterator is indexed to the `N+1`th element, it will alias the
second element, equivalent to index `1`. If a circular iterator is created to
the begining of a range and decremented once, it will alias the `end - 1`th
element of the range. And so on.

Due to this circular nature, `iter < iter + 1` may return `false` if
`iter + 1` wraps around the end of the range. The reverse holds true for
subtraction.
*/

#include <cstddef> // size_t, ptrdiff_t
#include <iterator>
#include <ranges>

#include <fgl/debug/constexpr_assert.hpp>

namespace fgl {

template <std::random_access_iterator T_iter>
class circular_iterator
{
	public:
	using difference_type = std::iter_difference_t<T_iter>;
	using value_type = std::iter_value_t<T_iter>;
	using reference_type = std::iter_reference_t<T_iter>;
	using iterator_category = std::random_access_iterator_tag;

	[[nodiscard]] constexpr explicit circular_iterator() = default;

	[[nodiscard]] constexpr explicit
	circular_iterator(T_iter begin, T_iter end, T_iter cursor)
	: m_begin(begin), m_end(end), m_iter(cursor)
	{
		FGL_DEBUG_CONSTEXPR_ASSERT(m_begin < m_end);
		FGL_DEBUG_CONSTEXPR_ASSERT(m_iter >= m_begin);
		FGL_DEBUG_CONSTEXPR_ASSERT(m_iter < m_end);
		using namespace std::ranges;
		static_assert(std::random_access_iterator<circular_iterator<T_iter>>);
		static_assert(common_range<circular_iterator<T_iter>>);
		static_assert(random_access_range<circular_iterator<T_iter>>);
	}

	[[nodiscard]] constexpr explicit
	circular_iterator(T_iter begin, T_iter end)
	: circular_iterator(begin, end, begin)
	{}

	// +size because end() might be a sentinel: need a common range

	[[nodiscard]] constexpr explicit
	circular_iterator(
		std::ranges::random_access_range auto& range,
		T_iter cursor)
	:
		circular_iterator(
			std::ranges::begin(range),
			std::ranges::begin(range) + std::ranges::size(range),
			cursor
		)
	{}

	[[nodiscard]] constexpr explicit
	circular_iterator(std::ranges::random_access_range auto& range)
	: circular_iterator(range, std::ranges::begin(range))
	{}

	[[nodiscard]] constexpr T_iter begin() const noexcept { return m_begin; }
	[[nodiscard]] constexpr T_iter end() const noexcept { return m_end; }
	[[nodiscard]] constexpr T_iter cursor() const noexcept { return m_iter; }

	// arithmetic

	[[nodiscard]] constexpr
	circular_iterator operator+(const circular_iterator& rhs) const noexcept
	{
		return circular_iterator(
			m_begin, m_end, relative_forward(m_iter - rhs.m_iter)
		);
	}

	[[nodiscard]] constexpr
	circular_iterator operator+(const difference_type n) const noexcept
	{ return circular_iterator(m_begin, m_end, relative_forward(n)); }

	constexpr circular_iterator& operator+=(const difference_type n) noexcept
	{
		m_iter = relative_forward(n);
		return *this;
	}

	constexpr circular_iterator& operator++() noexcept
	{
		if (m_iter == m_end - 1) m_iter = m_begin;
		else ++m_iter;
		return *this;
	}

	[[nodiscard]] constexpr circular_iterator operator++(int) noexcept
	{
		circular_iterator tmp = *this;
		++*this;
		return tmp;
	}

	[[nodiscard]] constexpr
	difference_type operator-(const circular_iterator& rhs) const noexcept
	{ return wrap_index(m_iter - rhs.m_iter); }

	[[nodiscard]] constexpr
	circular_iterator operator-(const difference_type n) const noexcept
	{ return circular_iterator(m_begin, m_end, relative_backward(n)); }

	constexpr circular_iterator& operator-=(const difference_type n)
	noexcept
	{
		m_iter = relative_backward(n);
		return *this;
	}

	constexpr circular_iterator& operator--() noexcept
	{
		if (m_iter == m_begin) m_iter = m_end - 1;
		else --m_iter;
		return *this;
	}

	[[nodiscard]] constexpr circular_iterator operator--(int) noexcept
	{
		circular_iterator tmp = *this;
		--*this;
		return tmp;
	}

	// dereference

	[[nodiscard]] constexpr reference_type operator*() noexcept
	{ return *m_iter; }

	[[nodiscard]] constexpr const reference_type operator*() const noexcept
	{ return *m_iter; }

	[[nodiscard]] constexpr
	reference_type operator[](const difference_type index) noexcept
	{ return *(m_begin + wrap_index(index)); }

	[[nodiscard]] constexpr
	const reference_type operator[](const difference_type index) const noexcept
	{ return *(m_begin + wrap_index(index)); }

	// comparisons

	[[nodiscard]] constexpr
	bool operator==(const circular_iterator& rhs) const noexcept
	{ return m_iter == rhs.m_iter; }

	[[nodiscard]] constexpr
	bool operator<(const circular_iterator& rhs) const noexcept
	{ return m_iter < rhs.m_iter; }

	[[nodiscard]] constexpr
	auto operator<=>(const circular_iterator&) const noexcept = default;

private:
	T_iter m_begin{ nullptr };
	T_iter m_end{ nullptr };
	T_iter m_iter{ nullptr };

	// index wrapping behavior

	[[nodiscard]] constexpr inline difference_type size() const noexcept
	{ return m_end - m_begin; }

	[[nodiscard]] constexpr
	difference_type wrap_index(const difference_type index) const noexcept
	{
		// TODO refactor this to use relative offsets rather than addresses
		if (const auto cursor{ m_iter + index };
			std::unsigned_integral<difference_type> || cursor >= m_begin)
		{
			return (cursor < m_end)
				? index
				: index % size();
		}
		else
		{
			const auto i{ index % size() };
			return i == 0 ? 0 : i + size();
		}
	}

	[[nodiscard]] constexpr
	T_iter relative_forward(const difference_type index) const noexcept
	{ return m_begin + wrap_index(index + (m_iter - m_begin)); }

	[[nodiscard]] constexpr
	T_iter relative_backward(const difference_type index) const noexcept
	{ return m_end - 1 - wrap_index(index + (m_end - 1 - m_iter)); }
};

template <std::random_access_iterator T_iter>
[[nodiscard]] constexpr circular_iterator<T_iter> operator+(
	const typename circular_iterator<T_iter>::difference_type n,
	const circular_iterator<T_iter>& iter) noexcept
{
	return iter + n;
}

static_assert(std::random_access_iterator<circular_iterator<char*>>);
static_assert(std::ranges::common_range<circular_iterator<char*>>);
static_assert(std::ranges::random_access_range<circular_iterator<char*>>);

// deduction guide
template <std::ranges::random_access_range T_range>
circular_iterator(T_range& range)
-> circular_iterator<decltype(std::ranges::begin(range))>;

/// helper for a circular range (infinite)

[[nodiscard]] constexpr
auto make_circular_range_alias(std::ranges::random_access_range auto& range)
{
	return std::ranges::subrange{
		circular_iterator{range},
		std::unreachable_sentinel
	};
}

} // namespace fgl

#endif // FGL_UTILITY_CIRCULAR_ITERATOR_HPP_INCLUDED
