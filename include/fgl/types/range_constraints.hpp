#pragma once
#ifndef FGL_TYPES_RANGE_CONSTRAINTS_HPP_INCLUDED
#define FGL_TYPES_RANGE_CONSTRAINTS_HPP_INCLUDED

#include <concepts>
#include <ranges>

#include "./traits.hpp"

/* FGL Constrained Range Concepts

Concepts that allow for constraining ranges based on value or reference type
without excessive `template` & `requires` boiler-plate noise.

FGL range constraints are conjunctions between a range concept and a type
concept which constrains either the `std::ranges::range_value_t` or
`std::ranges::range_reference_t`, depending on which is most applicable
to the concept.

Generated concept signatures:

template <T_range> concept
fgl::(RANGE CONCEPT NAME)_(TYPE CONCEPT NAME);

template <T_range, T_compare> concept
fgl::(RANGE CONCEPT NAME)_(TYPE CONCEPT NAME);

template <T_range, T_compare_range> concept
fgl::(RANGE CONCEPT NAME)_(TYPE CONCEPT NAME)_range;

**Range concepts**

- std::ranges::`range`
- std::ranges::`borrowed_range`
- std::ranges::`sized_range`
- std::ranges::`bidirectional_range`
- std::ranges::`random_access_range`
- std::ranges::`contiguous_range`
- std::ranges::`common_range`
- std::ranges::`viewable_range`

**Type concepts**

- std::`swappable_with<T&>`
- std::`assignable_from<T>`
- std::`same_as<T>`
- std::`derived_from<T>`
- std::`convertible_to<T>`
- std::`common_with<T>`
- std::`integral`
- std::`signed_integral`
- std::`unsigned_integral`
- std::`floating_point`
- fgl::traits::`not_same_as<T>`
- fgl::traits::`numeric_type`
- fgl::traits::`byte_type`
- fgl::traits::`pointer_to_byte`
- fgl::traits::`pointer_to_non_void`

**Type concepts and the `_range` suffix**

There are two categories of type constraints: "checks" that only take the
range template argument (i.e. `template <T_range>` like `..._integral`) and
"comparisons" which take an additional comparison type argument
(i.e. `template <T_range, T_compare>` like `..._same_as`).

Each comparison concept also has a variant with the `_range` suffix, whose
template arguments are `<T_range, T_compare_range>`. This variant will
retrieve the constraint type from `T_compare_range`'s value or reference
type; which ever is used to retrieve the type being constrained from `T_range`.
*/
/// Example program
/*
#include <iostream>
#include <utility> // swap
#include <ranges> // begin, end
#include <vector>
#include <array>

#include <fgl/types/range_constraints.hpp>

void simple_range_swap(
	std::ranges::range auto& r1,
	fgl::range_swappable_with_range<decltype(r1)> auto& r2)
{
	auto r1_iter{ std::ranges::begin(r1) };
	const auto r1_end{ std::ranges::end(r1) };
	auto r2_iter{ std::ranges::begin(r2) };
	const auto r2_end{ std::ranges::end(r2) };

	while (r1_iter != r1_end && r2_iter != r2_end)
		std::iter_swap(r1_iter++, r2_iter++);
}

void print_contiguous_int_range(
	const auto& prefix,
	const fgl::contiguous_range_same_as<int> auto& range)
{
	std::cout << prefix;
	for (const int v : range)
		std::cout << v << ' ';
	std::cout << '\n';
}

int main()
{
	std::vector a{ 1,2,3,4,5,6,7 };
	std::array b{ 7,6,5,4,3,2,1 };
	print_contiguous_int_range("a: ", a);
	print_contiguous_int_range("b: ", b);
	std::cout << "swap...\n";
	simple_range_swap(a, b);
	print_contiguous_int_range("a: ", a);
	print_contiguous_int_range("b: ", b);
}
*/
/// Example program output
/*
a: 1 2 3 4 5 6 7
b: 7 6 5 4 3 2 1
swap...
a: 7 6 5 4 3 2 1
b: 1 2 3 4 5 6 7
*/

#ifndef FGL_INTERNAL_CR_COMPARE
///////////////////////////////////////////////////////////////////////////////
	#define FGL_INTERNAL_CR_COMPARE(ns1, t1, ns2, t2, ns3, t3)\
		template <class T_range, class T_compare>\
		concept t1 ## _ ## t2 = ns1::t1<T_range>\
			&& ns2::t2<ns3::t3<T_range>, T_compare>;\
		template <class T_range, class T_compare_range>\
		concept t1 ## _ ## t2 ## _range = ns1::t1<T_range>\
			&& ns2::t2<ns3::t3<T_range>, ns3::t3<T_compare_range>>;
///////////////////////////////////////////////////////////////////////////////
#else
	#error FGL_INTERNAL_CR_COMPARE already defined
#endif

#ifndef FGL_INTERNAL_CR_COMPARE_V
///////////////////////////////////////////////////////////////////////////////
	#define FGL_INTERNAL_CR_COMPARE_V(ns1, t1, ns2, t2)\
		FGL_INTERNAL_CR_COMPARE(ns1, t1, ns2, t2, std::ranges, range_value_t)
///////////////////////////////////////////////////////////////////////////////
#else
	#error FGL_INTERNAL_CR_COMPARE_V already defined
#endif

#ifndef FGL_INTERNAL_CR_COMPARE_R
///////////////////////////////////////////////////////////////////////////////
	#define FGL_INTERNAL_CR_COMPARE_R(ns1, t1, ns2, t2)\
		FGL_INTERNAL_CR_COMPARE(\
			ns1, t1, ns2, t2, std::ranges, range_reference_t)
///////////////////////////////////////////////////////////////////////////////
#else
	#error FGL_INTERNAL_CR_COMPARE_R already defined
#endif
#ifndef FGL_INTERNAL_CR_CHECK
///////////////////////////////////////////////////////////////////////////////
	#define FGL_INTERNAL_CR_CHECK(ns1, t1, ns2, t2, ns3, t3)\
		template <class T_range>\
		concept t1 ## _ ## t2 = ns1::t1<T_range>\
			&& ns2::t2<ns3::t3<T_range>>;
///////////////////////////////////////////////////////////////////////////////
#else
	#error FGL_INTERNAL_CR_CHECK already defined
#endif

#ifndef FGL_INTERNAL_CR_CHECK_V
///////////////////////////////////////////////////////////////////////////////
	#define FGL_INTERNAL_CR_CHECK_V(ns1, t1, ns2, t2)\
		FGL_INTERNAL_CR_CHECK(ns1, t1, ns2, t2, std::ranges, range_value_t)
///////////////////////////////////////////////////////////////////////////////
#else
	#error FGL_INTERNAL_CR_CHECK_V already defined
#endif

#ifndef FGL_INTERNAL_CR_CHECK_R
///////////////////////////////////////////////////////////////////////////////
	#define FGL_INTERNAL_CR_CHECK_R(ns1, t1, ns2, t2)\
		FGL_INTERNAL_CR_CHECK(ns1, t1, ns2, t2, std::ranges, range_reference_t)
///////////////////////////////////////////////////////////////////////////////
#else
	#error FGL_INTERNAL_CR_CHECK_R already defined
#endif

#ifndef FGL_INTERNAL_CR_GENERATE_PERMUTATIONS
///////////////////////////////////////////////////////////////////////////////
	#define FGL_INTERNAL_CR_GENERATE_PERMUTATIONS(ns, range)\
	/* STD */\
	FGL_INTERNAL_CR_COMPARE_V(ns, range, std, same_as);\
	FGL_INTERNAL_CR_COMPARE_V(ns, range, std, derived_from);\
	FGL_INTERNAL_CR_COMPARE_V(ns, range, std, convertible_to);\
	FGL_INTERNAL_CR_COMPARE_V(ns, range, std, common_with);\
	FGL_INTERNAL_CR_COMPARE_R(ns, range, std, swappable_with);\
	FGL_INTERNAL_CR_COMPARE_R(ns, range, std, assignable_from);\
	FGL_INTERNAL_CR_CHECK_V(ns, range, std, integral);\
	FGL_INTERNAL_CR_CHECK_V(ns, range, std, signed_integral);\
	FGL_INTERNAL_CR_CHECK_V(ns, range, std, unsigned_integral);\
	FGL_INTERNAL_CR_CHECK_V(ns, range, std, floating_point);\
	/* FGL */\
	FGL_INTERNAL_CR_COMPARE_V(ns, range, fgl::traits, not_same_as);\
	FGL_INTERNAL_CR_CHECK_V(ns, range, fgl::traits, numeric_type);\
	FGL_INTERNAL_CR_CHECK_V(ns, range, fgl::traits, byte_type);\
	FGL_INTERNAL_CR_CHECK_V(ns, range, fgl::traits, pointer_to_byte);\
	FGL_INTERNAL_CR_CHECK_V(ns, range, fgl::traits, pointer_to_non_void);
///////////////////////////////////////////////////////////////////////////////
#else
	#error FGL_INTERNAL_CR_GENERATE_PERMUTATIONS already defined
#endif

namespace fgl {

FGL_INTERNAL_CR_GENERATE_PERMUTATIONS(std::ranges, range);
FGL_INTERNAL_CR_GENERATE_PERMUTATIONS(std::ranges, borrowed_range);
FGL_INTERNAL_CR_GENERATE_PERMUTATIONS(std::ranges, sized_range);
FGL_INTERNAL_CR_GENERATE_PERMUTATIONS(std::ranges, forward_range);
FGL_INTERNAL_CR_GENERATE_PERMUTATIONS(std::ranges, bidirectional_range);
FGL_INTERNAL_CR_GENERATE_PERMUTATIONS(std::ranges, random_access_range);
FGL_INTERNAL_CR_GENERATE_PERMUTATIONS(std::ranges, contiguous_range);
FGL_INTERNAL_CR_GENERATE_PERMUTATIONS(std::ranges, common_range);
FGL_INTERNAL_CR_GENERATE_PERMUTATIONS(std::ranges, viewable_range);

} // namespace fgl

#endif // FGL_TYPES_RANGE_CONSTRAINTS_HPP_INCLUDED
