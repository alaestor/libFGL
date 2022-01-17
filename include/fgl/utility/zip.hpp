#pragma once
#ifndef FGL_UTILITY_ZIP_HPP_INCLUDED
#define FGL_UTILITY_ZIP_HPP_INCLUDED

/// QUICK-START GUIDE
/*
    for (const auto& [a, b] : zip(a_vec, b_arr))
		a += b; // mutable references like *begin()

	use fgl::czip for const references like *cbegin()

	Returns a range iterating multiple ranges of differing types.
	Iterates based on the smallest range size.
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

#include "../types/range_wrapper.hpp"

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

	// Getting a redefinition error? T_iters mustn't be empty.
	[[nodiscard]] explicit constexpr forward_zip_iterator() noexcept = default;

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

static_assert(std::forward_iterator<forward_zip_iterator<char*>>);
static_assert(std::forward_iterator<forward_zip_iterator<const char*>>);
static_assert(
	std::sentinel_for
	<
		forward_zip_iterator<const double*>::difference_type,
		forward_zip_iterator<const double*>
	>
);

using forward_zip_sentinel_t = forward_zip_iterator<char*>::difference_type;

}// namespace internal

/*Returns a range iterating multiple ranges of differing types.
Example usage: `for (const auto& [v1,v2,v3] : czip(a,b,c)) v1 += v2 + v3;`*/
template<std::ranges::forward_range ... T_ranges>
constexpr auto zip(T_ranges&& ... args)
{
	using fgl::internal::forward_zip_iterator;
	const zip_sentinel_t shortest{
		std::min({static_cast<std::ptrdiff_t>(std::ranges::ssize(args))...})
	};
	return fgl::range_wrapper(
		forward_zip_iterator(std::begin(std::forward<T_ranges>(args))...),
		shortest
	);
}

/*Returns a range iterating multiple const ranges of differing types.
Example usage: `for (const auto& [v1,v2,v3] : czip(a,b,c)) func(v1,v2,v3);`*/
template<std::ranges::forward_range ... T_ranges>
constexpr auto czip(T_ranges&& ... args)
{
	using fgl::internal::forward_zip_iterator;
	const zip_sentinel_t shortest{
		std::min({static_cast<std::ptrdiff_t>(std::ranges::ssize(args))...})
	};
	return fgl::range_wrapper(
		forward_zip_iterator(std::cbegin(std::forward<T_ranges>(args))...),
		shortest
	);
}

} // namespace fgl

#endif // FGL_UTILITY_ZIP_HPP_INCLUDED
