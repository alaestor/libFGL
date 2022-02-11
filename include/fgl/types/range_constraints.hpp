#pragma once
#ifndef FGL_TYPES_RANGE_CONSTRAINTS_HPP_INCLUDED
#define FGL_TYPES_RANGE_CONSTRAINTS_HPP_INCLUDED

#include <concepts>
#include <ranges>

#include "./traits.hpp"

/* FGL Constrained Range Concepts

At the time of writing, concepts cannot be passed as template arguments.
This isn't a one-size-fits-all solution; trying to reduce boilerplate noise.

Generated concept names concantinates range and reference type constraints

	`range_same_as<T>` combines `std::ranges::range` and `std::same_as`

	`contiguous_range_convertible_from<T>` combines
		`std::ranges::contiguous_range` and `std::convertible_from`

Range concepts:
	range
	borrowed_range
	sized_range
	forward_range
	bidirectional_range
	random_access_range
	contiguous_range
	common_range
	viewable_range

Reference type concepts: (required with std::ranges::range_reference_t)
	same_as
	derived_from
	convertible_to
	common_with
	assignable_from
	swappable_with

Value type concepts: (required with std::ranges::range_value_t)
	integral
	signed_integral
	unsigned_integral
	floating_point

libFGL value type concepts: (required with std::ranges::range_value_t)
	not_same_as
	byte_type
	numeric_type
	pointer_to_byte
	pointer_to_non_void

Combine a range concept and type concept:
	bidirectional_range_derived_from<T>
	viewable_range_integral
	forward_range_swappable_with<T>
	etc.
*/

#ifndef FGL_INTERNAL_CR_COMPARE
///////////////////////////////////////////////////////////////////////////////
	#define FGL_INTERNAL_CR_COMPARE(ns1, t1, ns2, t2, ns3, t3)\
		template <class T_range, class T_compare>\
		concept t1 ## _ ## t2 = ns1::t1<T_range>\
			&& ns2::t2<ns3::t3<T_range>, T_compare>;
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
