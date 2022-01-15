#pragma once
#ifndef FGL_TYPE_TRAITS_HPP_INCLUDED
#define FGL_TYPE_TRAITS_HPP_INCLUDED

#include <cstddef> // byte
#include <type_traits>
#include <concepts>

namespace fgl::traits {

template <typename T>
concept byte_type =
	std::same_as<std::remove_cv_t<T>, std::byte>
	|| std::same_as<std::remove_cv_t<T>, char>
	|| std::same_as<std::remove_cv_t<T>, unsigned char>
; // no one should use signed char for raw bytes...

template<typename T>
concept numeric_type = std::integral<T> || std::floating_point<T>;

/// Pointers

template <typename T>
struct remove_all_pointers : std::conditional_t
< // klaus triendl @ https://stackoverflow.com/a/39492671
    std::is_pointer_v<T>,
    remove_all_pointers<std::remove_pointer_t<T>>,
	std::type_identity<T>
>
{};

template <typename T>
using remove_all_pointers_t = typename remove_all_pointers<T>::type;

template <typename T>
using remove_cvptr_t = typename std::remove_cv_t<std::remove_pointer_t<T>>;

template <typename T>
concept pointer_to_byte =
	std::is_pointer_v<T>
	&& (
		byte_type<std::remove_pointer_t<T>>
		|| std::same_as<remove_cvptr_t<T>, void>
	)
;

/// Reference

template <typename T, typename U = T>
inline constexpr bool is_const_ref{
	std::is_reference_v<T> && std::is_const_v<std::remove_reference_t<T>>
};

template <typename T>
inline constexpr bool is_nonconst_ref{
	std::is_reference_v<T> && !std::is_const_v<std::remove_reference_t<T>>
};


/// Concept helpers

/* NOT ADVISED!
Diagnostics won't tell you which concept failed :(
This hack exists because concepts can't be template params.*/
template <auto T_concept_lambda, typename T>
inline constexpr bool satisfies_concept_lambda{
    T_concept_lambda.template operator()<T>()
};

template <auto T_concept_lambda, typename T>
concept for_each_cv_permutation =
	satisfies_concept_lambda<T_concept_lambda, T>
	&& satisfies_concept_lambda<T_concept_lambda, const T>
	&& satisfies_concept_lambda<T_concept_lambda, volatile T>
	&& satisfies_concept_lambda<T_concept_lambda, const volatile T>
;

template <auto T_concept_lambda, typename T>
concept for_each_cvptr_permutation =
	for_each_cv_permutation<T_concept_lambda, T*>
	&& for_each_cv_permutation<T_concept_lambda, T* const>
	&& for_each_cv_permutation<T_concept_lambda, T* volatile>
	&& for_each_cv_permutation<T_concept_lambda, T* const volatile>
;

/*
example:

constexpr auto pointer_to_byte_concept_lambda{
    []<typename T>() consteval { return pointer_to_byte<T>; }
};

template <typename T>
concept can_be_byte_pointer_type =
    for_each_cvptr_permutation<pointer_to_byte_concept_lambda, T>;

static_assert(can_be_byte_pointer_type<std::byte>);
static_assert(!can_be_byte_pointer_type<int>);

// is like doing:
//		concept<T*>
//		&& concept<T* const>
//		&& concept<T* volatile>
//		&& concept<T* const volatile>
//		&& concept<const T*>
//		&& concept<const T* const>
//		&& concept<const T* volatile>
//		&& concept<const T* const volatile>
//		&& concept<volatile T*>
//		&& concept<volatile T* const>
//		&& concept<volatile T* volatile>
//		&& concept<volatile T* const volatile>
//		&& concept<const volatile T*>
//		&& concept<const volatile T* const>
//		&& concept<const volatile T* volatile>
//		&& concept<const volatile T* const volatile>
*/


} // namespace fgl::traits

#endif // FGL_TYPE_TRAITS_HPP_INCLUDED
