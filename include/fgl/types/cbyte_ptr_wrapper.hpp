#pragma once
#ifndef FGL_TYPES_CBYTE_PTR_WRAPPER_HPP_INCLUDED
#define FGL_TYPES_CBYTE_PTR_WRAPPER_HPP_INCLUDED

/// QUICK-START GUIDE
/*
	for easy conversion between C/C++ pointer types.
	implicit conversions can be disabled via template.

	// implicit conversions enabled by default
	cbyte_ptr_wrapper ptr(p);
	void* vptr = ptr;
	char* cptr = ptr;
	unsigned char* ucptr = ptr;
	std::byte* bptr = ptr;

	// implicit conversions disabled by template
	cbyte_ptr_wrapper<void*, false> ptr(p);
	void* vptr = ptr.as<void*>();
	char* cptr = ptr.as<char*>();
	unsigned char* ucptr = ptr.as<unsigned char*>();
	std::byte* bptr = ptr.as<std::byte*>();
*/

#include "./traits.hpp"

namespace fgl {

/*A simple casting wrapper for a non-owning byte pointer to simplify C-style
byte interfaces. Implicit conversions are enabled by default, but can be
disabled via boolean `false` template parameter.*/
template
<
	fgl::traits::pointer_to_byte T,
	bool enable_implicit_conversions = true
>
struct cbyte_ptr_wrapper;

/// deduction guide
template<fgl::traits::pointer_to_byte T>
cbyte_ptr_wrapper(T) -> cbyte_ptr_wrapper<T, true>;

template <fgl::traits::pointer_to_byte T>
struct cbyte_ptr_wrapper<T, false>
{
	T pointer{ nullptr };

	// This isn't a smart pointer. Argument should be non-owning.
	[[nodiscard]] explicit // not constexpr, no point; can't reinterpret cast
	cbyte_ptr_wrapper(T non_owning_pointer) noexcept
	: pointer(non_owning_pointer)
	{}

	// reinterpret cast wrapper
	template <fgl::traits::pointer_to_byte T_cast>
	[[nodiscard]] T_cast as() const noexcept
	{ return reinterpret_cast<T_cast>(pointer); }
};

template <fgl::traits::pointer_to_byte T>
struct cbyte_ptr_wrapper<T, true> : public cbyte_ptr_wrapper<T, false>
{
	// This isn't a smart pointer. Argument should be non-owning.
	[[nodiscard]] explicit // not constexpr, no point; can't reinterpret cast
	cbyte_ptr_wrapper(T non_owning_pointer) noexcept
	: cbyte_ptr_wrapper<T, false>(non_owning_pointer)
	{}

	// implicit reinterpret cast conversion operator
	template <fgl::traits::pointer_to_byte T_cast>
	[[nodiscard]] operator T_cast() const noexcept
	{ return reinterpret_cast<T_cast>(this->pointer); }
};

} // namespace fgl

#endif // FGL_TYPES_CBYTE_PTR_WRAPPER_HPP_INCLUDED



