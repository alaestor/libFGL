#pragma once
#ifndef FGL_UTILITY_MAKE_BYTE_ARRAY_HPP_INCLUDED
#define FGL_UTILITY_MAKE_BYTE_ARRAY_HPP_INCLUDED
#include "../environment/libfgl_compatibility_check.hpp"

/// QUICK-START GUIDE
/*
	auto barray1 = fgl::make_byte_array( "a cstring literal" );

	using namespace fgl::literals;
	auto barray2 = "a cstring literal"_bytes;

	Creates a std::array of std::byte from cstring literal (excludes null term)
*/
/// EXAMPLE PROGRAM
/*
	#include <iostream>
	#include <cstddef> // byte, to_integer

	#include <fgl/utility/make_byte_array.hpp>

	int main()
	{
		constexpr auto barray1 = fgl::make_byte_array("\x01\xA0\xAF\xFF hello");
		for (const std::byte b : barray1)
			std::cout << "0x" << std::to_integer<int>(b) << ' ';

		std::cout << std::endl;

		using namespace fgl::literals;
		constexpr auto barray2 = "\x01\xA0\xAF\xFF goodbye"_bytes;
		for (const std::byte b : barray2)
			std::cout << "0x" << std::to_integer<int>(b) << ' ';
	}
*/
/// EXAMPLE OUTPUT
/*
0x1 0x160 0x175 0x255 0x32 0x104 0x101 0x108 0x108 0x111
0x1 0x160 0x175 0x255 0x32 0x103 0x111 0x111 0x100 0x98 0x121 0x101
*/

#include <cstddef> // size_t, byte
#include <utility> // index_sequence, make_index_sequence
#include <algorithm> // copy
#include <array>
#include "../types/ctypes.hpp"

namespace fgl {
// returns a std::array of std::byte from cstring literal (excludes null term).
template <std::size_t T_length> [[nodiscard]] constexpr
std::array<std::byte, T_length-1> make_byte_array(
	fgl::carray<const char, T_length>& cstr)
{
	constexpr std::size_t arrayLength{ T_length - 1 }; // no null term
	return
		[&]<std::size_t ... T_indexes>(std::index_sequence<T_indexes...>)
		constexpr -> std::array<std::byte, arrayLength>
		{
			return {{static_cast<std::byte>(cstr[T_indexes])...}};
		}(std::make_index_sequence<arrayLength>());
}
} // namespace fgl

namespace fgl::literals {
namespace internal {
// this only exists because operator"" cant take a cstring literal (array ref)
template <std::size_t N>
struct byte_array_literal_helper final
{
	using char_array_t = fgl::carray<char, N>;
	char_array_t char_array{};

	[[nodiscard]] consteval
	byte_array_literal_helper(const char_array_t& cstring_literal) noexcept
	{
		std::ranges::copy(cstring_literal, char_array);
	}
};
} // namespace internal

// returns a std::array of std::byte from cstring literal (excludes null term).
template<internal::byte_array_literal_helper T_container>
consteval auto operator"" _bytes()
{
	return make_byte_array(T_container.char_array);
}
} // namespace fgl::literals

#endif // FGL_UTILITY_MAKE_BYTE_ARRAY_HPP_INCLUDED
