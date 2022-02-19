#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE, byte
#include <type_traits> // is_same_v

#define FGL_SHORT_MACROS
#include <fgl/debug/constexpr_assert.hpp>

#include <fgl/types/cbyte_ptr_wrapper.hpp>

#ifdef NDEBUG
	#error NDEBUG must not be defined for tests because they rely on assertions
#endif // NDEBUG

using fgl::cbyte_ptr_wrapper;

bool test_cbyte_ptr_wrapper_explicit()
{
	char c{ 'c' };
	const cbyte_ptr_wrapper<decltype(c)*, false> ptr( &c );
	[[maybe_unused]] void* vp{ ptr.as<void*>() };
	[[maybe_unused]] char* cp{ ptr.as<char*>() };
	[[maybe_unused]] unsigned char* up{ ptr.as<unsigned char*>() };
	[[maybe_unused]] std::byte* bp{ ptr.as<std::byte*>() };
	return true;
}

bool test_cbyte_ptr_wrapper_explicit_const()
{
	const char c{ 'c' };
	const cbyte_ptr_wrapper<decltype(c)*, false> ptr( &c );
	[[maybe_unused]] const void* vp{ ptr.as<const void*>() };
	[[maybe_unused]] const char* cp{ ptr.as<const char*>() };
	[[maybe_unused]] const unsigned char* up{ ptr.as<const unsigned char*>() };
	[[maybe_unused]] const std::byte* bp{ ptr.as<const std::byte*>() };
	return true;
}

bool test_cbyte_ptr_wrapper_implicit()
{
	char c{ 'c' };
	const cbyte_ptr_wrapper<decltype(c)*, true> ptr( &c );
	[[maybe_unused]] void* vp{ ptr };
	[[maybe_unused]] char* cp{ ptr };
	[[maybe_unused]] unsigned char* uc{ ptr };
	[[maybe_unused]] std::byte* bp{ ptr };
	return true;
}

bool test_cbyte_ptr_wrapper_implicit_const()
{
	const char c{ 'c' };
	const cbyte_ptr_wrapper<decltype(c)*, true> ptr( &c );
	[[maybe_unused]] const void* vp{ ptr };
	[[maybe_unused]] const char* cp{ ptr };
	[[maybe_unused]] const unsigned char* uc{ ptr };
	[[maybe_unused]] const std::byte* bp{ ptr };
	return true;
}

int main()
{
	constexpr_assert(test_cbyte_ptr_wrapper_explicit());
	constexpr_assert(test_cbyte_ptr_wrapper_explicit_const());
	constexpr_assert(test_cbyte_ptr_wrapper_implicit());
	constexpr_assert(test_cbyte_ptr_wrapper_implicit_const());
	return EXIT_SUCCESS;
}
