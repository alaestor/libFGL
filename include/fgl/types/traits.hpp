#pragma once
#ifndef FGL_TYPE_TRAITS_HPP_INCLUDED
#define FGL_TYPE_TRAITS_HPP_INCLUDED

#include <cstddef> // byte
#include <type_traits>
#include <ranges>
#include <concepts>

namespace fgl::traits {

template <typename T>
concept byte_type =
	std::same_as<std::remove_cv_t<T>, std::byte>
	|| std::same_as<std::remove_cv_t<T>, char>
	|| std::same_as<std::remove_cv_t<T>, unsigned char>
; // no one should use signed char for raw bytes...

template <typename T>
concept cbyte_type = byte_type<T> || std::same_as<std::remove_cv_t<T>, void>;

template <typename T>
concept numeric_type = std::integral<T> || std::floating_point<T>;

template <typename T1, typename T2>
concept not_same_as = (std::same_as<T1, T2> == false);


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

template <typename T>
concept pointer_to_non_void =
	std::is_pointer_v<T>
	&& (!std::is_void_v<std::remove_cv_t<remove_all_pointers_t<T>>>)
;


/// Reference

template <typename T>
inline constexpr bool is_const_ref{
	std::is_reference_v<T> && std::is_const_v<std::remove_reference_t<T>>
};

template <typename T>
inline constexpr bool is_nonconst_ref{
	std::is_reference_v<T> && !std::is_const_v<std::remove_reference_t<T>>
};


} // namespace fgl::traits

#endif // FGL_TYPE_TRAITS_HPP_INCLUDED
