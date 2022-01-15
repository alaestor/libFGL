#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE, byte
#include <type_traits> // is_same_v

#include <fgl/types/cbyte_ptr_wrapper.hpp>

#define FGL_SHORT_MACROS
#include <fgl/debug/constexpr_assert.hpp>

#ifdef NDEBUG
	#error NDEBUG must not be defined for tests because they rely on assertions
#endif // NDEBUG

using fgl::cbyte_ptr_wrapper;

bool test_cbyte_ptr_wrapper_explicit()
{
	char c{ 'c' };
	cbyte_ptr_wrapper ptr( &c );
	[[maybe_unused]] void* vp{ ptr.as_void() };
	[[maybe_unused]] char* cp{ ptr.as_char() };
	[[maybe_unused]] unsigned char* uc{ ptr.as_uchar() };
	[[maybe_unused]] std::byte* bp{ ptr.as_byte() };
	[[maybe_unused]] const void* vpc{ ptr.as_void_const() };
	[[maybe_unused]] const char* cpc{ ptr.as_char_const() };
	[[maybe_unused]] const unsigned char* ucc{ ptr.as_uchar_const() };
	[[maybe_unused]] const std::byte* bpc{ ptr.as_byte_const() };
	return true;
}

bool test_cbyte_ptr_wrapper_explicit_const()
{
	const char c{ 'c' };
	const cbyte_ptr_wrapper ptr( &c );
	[[maybe_unused]] const void* vp{ ptr.as_void() };
	[[maybe_unused]] const char* cp{ ptr.as_char() };
	[[maybe_unused]] const unsigned char* uc{ ptr.as_uchar() };
	[[maybe_unused]] const std::byte* bp{ ptr.as_byte() };
	return true;
}

bool test_cbyte_ptr_wrapper_implicit()
{
	char c{ 'c' };
	cbyte_ptr_wrapper<decltype(c)*, true> ptr( &c );
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
