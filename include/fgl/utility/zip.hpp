#pragma once
#ifndef FGL_UTILITY_ZIP_HPP_INCLUDED
#define FGL_UTILITY_ZIP_HPP_INCLUDED
#include "../environment/libfgl_compatibility_check.hpp"

/// QUICK-START GUIDE
/*
	for (const auto& [a, b] : zip(a_vec, b_arr))
		a += b; // mutable references like *begin()

	int sum{ 0 };
	for (const auto& [a, b] : czip(a_vec, b_arr))
		sum += a + b; // const references like *cbegin()

	// explicit length so it doesn't need to check each range
	for (const auto& [a, b, c]
		: czip(shortest_range.size(), shortest_range, longer, longer))

	An interface to easily create a range which iterates multiple ranges of
	different types at once. The range is composed of an FGL internal zip
	iterator and a length-based end sentinel. Dereferencing the iterator
	returns a tuple of references or values from the zipped ranges' iterators,
	in the order the ranges were passed to `fgl::zip`. In the case of
	`fgl::czip`, they are `const`-qualified references.
*/
/// EXAMPLE PROGRAM
/*
#include <iostream>
#include <array>
#include <vector>
#include <map>

#include <fgl/utility/zip.hpp>

consteval int calc()
{
	int sum{ 0 };
	std::array a1{ 0,1,2,3,4 };
	std::array a2{ 5,6,7,8,9,10,2000,4500001 };
	std::array a3{ 11,12,13,14,15,16,251000,4500,145101 };
	for (const auto& [a1_v, a2_v, a3_v] : fgl::czip(a1, a2,a3))
		sum += a1_v + a2_v + a3_v;
	return sum;
}

int main()
{
	constexpr auto result{ calc() };
	std::cout << result << std::endl;

	std::vector<float> vec(20, 3.14f);
	std::map<int, float> map{ {1, 1.1f}, {2, 2.2f}, {3, 3.3f} };
	for (const auto& [v1, m, v2] : fgl::zip(vec, map, vec))
	{
		const auto& [m_k, m_v]{ m }; // cant nest structured bindings
		std::cout<< v1 << " += " << m_k << " + " << m_v << " = ";
		v1 += m_k + m_v;
		std::cout << v2 << '\n'; // same element as v1
	}
}
*/
/// EXAMPLE OUTPUT
/*
110
3.14 += 1 + 1.1 == 5.24
3.14 += 2 + 2.2 == 7.34
3.14 += 3 + 3.3 == 9.44
*/

#include <cstddef> // size_t, ptrdiff_t
#include <functional> // ref
#include <iterator>
#include <tuple>

#include "../types/range_alias.hpp"
#include "../debug/constexpr_assert.hpp"

namespace fgl {

using zip_sentinel_t = std::ptrdiff_t;

namespace internal {

template <std::forward_iterator ... T_iters>
class forward_zip_iterator final
{
	zip_sentinel_t m_index{ 0 };
	std::tuple<T_iters...> m_iters;

	// maybe an easier way but oh well
	[[nodiscard]] static constexpr auto accessType(auto& v) noexcept
	{ return std::ref(v); }

	[[nodiscard]] static constexpr auto accessType(auto&& v) noexcept
	{ return v; }

	template <std::forward_iterator T>
	using get_iter_ref_t = typename std::iterator_traits<T>::reference;

public:
	using value_type = std::tuple<get_iter_ref_t<T_iters>...>;
	using difference_type = decltype(m_index);
	using iterator_category = std::forward_iterator_tag;

	[[nodiscard]] explicit constexpr forward_zip_iterator() noexcept
		= default; // Getting a redefinition error? T_iters mustn't be empty.

	[[nodiscard]] explicit constexpr forward_zip_iterator(T_iters&& ... args)
	: m_iters(std::forward<T_iters>(args)...)
	{ static_assert(std::forward_iterator<forward_zip_iterator<T_iters...>>); }

	constexpr forward_zip_iterator& operator++() noexcept
	{
		++m_index;
		std::apply([](auto && ... args) noexcept { ((++args),...); }, m_iters);
		return *this;
	}

	// Use ++prefix if you don't want the temporary.
	[[nodiscard]] constexpr forward_zip_iterator operator++(int)
	{
		forward_zip_iterator tmp = *this;
		++*this;
		return tmp;
	}

	// Use a difference_type sentinel! This comparison is stupid and expensive!
	[[nodiscard]] [[deprecated("Expensive & internal. Use difference_type.")]]
	constexpr bool operator==(const forward_zip_iterator& rhs) const
	{
		const auto any_iterators_match{
			[&]<std::size_t ... i>(std::index_sequence<i...>) constexpr -> bool
			{ return ((std::get<i>(m_iters)==std::get<i>(rhs.m_iters))||...); }
		};
		return any_iterators_match(std::index_sequence_for<T_iters...>());
	}

	[[nodiscard]] constexpr
	bool operator==(const difference_type index) const noexcept
	{ return m_index == index; }

	[[nodiscard]] constexpr
	auto operator<=>(const difference_type index) const noexcept
	{ return m_index <=> index; }

	[[nodiscard]] constexpr difference_type index() const noexcept
	{ return m_index; }

	[[nodiscard]] constexpr value_type operator*() const
	noexcept( (noexcept(*T_iters{}) && ...) )
	{
		return [&]<std::size_t ... i>(std::index_sequence<i...>) -> value_type
		{
			return std::tuple(accessType(*std::get<i>(m_iters))...);
		}(std::index_sequence_for<T_iters...>());
	}
};


// sanity checks

static_assert(std::forward_iterator<forward_zip_iterator<char*>>);
static_assert(std::forward_iterator<forward_zip_iterator<const char*>>);
static_assert(
	std::sentinel_for
	<
		forward_zip_iterator<const double*>::difference_type,
		forward_zip_iterator<const double*>
	>
);

//using forward_zip_sentinel_t = forward_zip_iterator<char*>::difference_type;

// utility functions

[[nodiscard]] constexpr inline
zip_sentinel_t shortest(const std::integral auto& ... lengths)
{
	return std::min({static_cast<zip_sentinel_t>(lengths)...});
}

}// namespace internal

/// fgl::zip public interface
// should these even bother forwarding?

/*Returns a range which iterates over multiple ranges of different types.
The zip range's sentinel is determined by the `length` parameter, which is
converted to a signed integral (zip_sentinel_t) via static_cast.
Example usage: `for (const auto& [va,vb,vc] : zip(a.size(), a, b, c))`*/
template <std::ranges::forward_range ... T_ranges>
constexpr auto zip(const std::integral auto length, T_ranges&& ... args)
{
	FGL_DEBUG_CONSTEXPR_ASSERT(
		static_cast<zip_sentinel_t>(length)
		<= internal::shortest(std::ranges::ssize(args)...)
	);
	using fgl::internal::forward_zip_iterator;
	return fgl::range_alias(
		forward_zip_iterator(std::begin(std::forward<T_ranges>(args))...),
		static_cast<zip_sentinel_t>(length)
	);
}

/*Returns a range which iterates over multiple ranges of different types.
The zip range's sentinel is determined by the smallest range.
Example usage: `for (const auto& [va,vb,vc] : zip(a, b, c))`*/
template <std::ranges::forward_range ... T_ranges>
constexpr auto zip(T_ranges&& ... args)
{
	using internal::shortest, std::ranges::ssize;
	return fgl::zip(shortest(ssize(args)...), std::forward<T_ranges>(args)...);
}

/// czip


/*Returns a range which iterates over multiple const ranges of different types.
The zip range's sentinel is determined by the `length` parameter, which is
converted to a signed integral (zip_sentinel_t) via static_cast.
Example usage: `for (const auto& [va,vb,vc] : zip(a.size(), a, b, c))`*/
template <std::ranges::forward_range ... T_ranges>
constexpr auto czip(const std::integral auto length, T_ranges&& ... args)
{
	FGL_DEBUG_CONSTEXPR_ASSERT(
		static_cast<zip_sentinel_t>(length)
		<= internal::shortest(std::ranges::ssize(args)...)
	);
	using fgl::internal::forward_zip_iterator;
	return fgl::range_alias(
		forward_zip_iterator(std::cbegin(std::forward<T_ranges>(args))...),
		static_cast<zip_sentinel_t>(length)
	);
}

/*Returns a range which iterates over multiple const ranges of different types.
const iterators. The zip range's sentinel is determined by the smallest range.
Example usage: `for (const auto& [va,vb,vc] : zip(a, b, c))`*/
template <std::ranges::forward_range ... T_ranges>
constexpr auto czip(T_ranges&& ... args)
{
	using internal::shortest, std::ranges::ssize;
	return fgl::czip(shortest(ssize(args)...), std::forward<T_ranges>(args)...);
}

} // namespace fgl

#endif // FGL_UTILITY_ZIP_HPP_INCLUDED
