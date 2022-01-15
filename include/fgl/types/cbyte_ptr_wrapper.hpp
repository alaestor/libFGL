#pragma once
#ifndef FGL_TYPES_CBYTE_PTR_WRAPPER_HPP_INCLUDED
#define FGL_TYPES_CBYTE_PTR_WRAPPER_HPP_INCLUDED

/// QUICK-START GUIDE
/*
	for easy conversion between C/C++ pointer types
	optional implicit conversions via template

	cbyte_ptr_wrapper ptr(c_void_ptr); // no implicit conversions
	void* vptr = ptr.as_void();
	char* cptr = ptr.as_char();
	unsigned char* ucptr = ptr.unsigned_char();
	std::byte* bptr = ptr.as_byte();

	cbyte_ptr_wrapper<void*, true> ptr(c_void_ptr); // implicit conversions
	void* vptr = ptr;
	char* cptr = ptr;
	unsigned char* ucptr = ptr;
	std::byte* bptr = ptr;
*/

#include "../types/traits.hpp"

namespace fgl {

/*A simple casting wrapper for a non-owning byte pointer. Makes working with
C-style interface easier. void*, char*, unsigned char*, and std::byte*
Implicit conversions can be enabled through a boolean true template argument.*/
template
<
	fgl::traits::pointer_to_byte T,
	bool enable_implicit_conversions = false
>
struct cbyte_ptr_wrapper;

/// deduction guide
template<fgl::traits::pointer_to_byte T>
cbyte_ptr_wrapper(T) -> cbyte_ptr_wrapper<T, false>;

template <fgl::traits::pointer_to_byte T>
struct cbyte_ptr_wrapper<T, false>
{
	using value_type = T;
	using reference_type = value_type&;
	using const_reference_type = const reference_type;

	value_type ptr{ nullptr };

	// This isn't a smart pointer. Argument should be non-owning.
	[[nodiscard]] explicit constexpr
	cbyte_ptr_wrapper(T non_owning_pointer) noexcept
	: ptr(non_owning_pointer)
	{}

	[[nodiscard]] void* as_void() noexcept
	{ return reinterpret_cast<void*>(ptr); }

	[[nodiscard]] const void* as_void() const noexcept
	{ return reinterpret_cast<const void*>(ptr); }

	[[nodiscard]] const void* as_void_const() const noexcept
	{ return as_void(); }

	[[nodiscard]] char* as_char() noexcept
	{ return reinterpret_cast<char*>(ptr); }

	[[nodiscard]] const char* as_char() const noexcept
	{ return reinterpret_cast<const char*>(ptr); }

	[[nodiscard]] const char* as_char_const() const noexcept
	{ return as_char(); }

	[[nodiscard]] unsigned char* as_uchar() noexcept
	{ return reinterpret_cast<unsigned char*>(ptr); }

	[[nodiscard]] const unsigned char* as_uchar() const noexcept
	{ return reinterpret_cast<const unsigned char*>(ptr); }

	[[nodiscard]] const unsigned char* as_uchar_const() const noexcept
	{ return as_uchar(); }

	[[nodiscard]] std::byte* as_byte() noexcept
	{ return reinterpret_cast<std::byte*>(ptr); }

	[[nodiscard]] const std::byte* as_byte() const noexcept
	{ return reinterpret_cast<const std::byte*>(ptr); }

	[[nodiscard]] const std::byte* as_byte_const() const noexcept
	{ return as_byte(); }
};

template <fgl::traits::pointer_to_byte T>
struct cbyte_ptr_wrapper<T, true> : public cbyte_ptr_wrapper<T, false>
{
	using B = cbyte_ptr_wrapper<T, false>;
	using
		B::ptr,
		B::as_void,
		B::as_char,
		B::as_uchar,
		B::as_byte;

	// This isn't a smart pointer. Argument should be non-owning.
	[[nodiscard]] explicit constexpr
	cbyte_ptr_wrapper(T non_owning_pointer) noexcept
	: cbyte_ptr_wrapper<T, false>(non_owning_pointer)
	{}

	[[nodiscard]] operator void*() noexcept
	{ return as_void(); }

	[[nodiscard]] operator const void*() const noexcept
	{ return as_void(); }

	[[nodiscard]] operator char*() noexcept
	{ return as_char(); }

	[[nodiscard]] operator const char*() const noexcept
	{ return as_char(); }

	[[nodiscard]] operator unsigned char*() noexcept
	{ return as_uchar(); }

	[[nodiscard]] operator const unsigned char*() const noexcept
	{ return as_uchar(); }

	[[nodiscard]] operator std::byte*() noexcept
	{ return as_byte(); }

	[[nodiscard]] operator const std::byte*() const noexcept
	{ return as_byte(); }
};

} // namespace fgl

#endif // FGL_TYPES_CBYTE_PTR_WRAPPER_HPP_INCLUDED
