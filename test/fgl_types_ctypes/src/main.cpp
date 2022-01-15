#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE, byte
#include <type_traits> // is_same_v

#include <fgl/types/ctypes.hpp>

#define FGL_SHORT_MACROS
#include <fgl/debug/constexpr_assert.hpp>

#ifdef NDEBUG
	#error NDEBUG must not be defined for tests because they rely on assertions
#endif // NDEBUG

consteval bool test_static_ctypes()
{
	using namespace fgl::ctypes;
	static_assert(std::is_same_v<cstring, const char*>);
	static_assert(std::is_same_v<cbyte, unsigned char>);
	return true;
}

int main()
{
	static_assert(test_static_ctypes());
	return EXIT_SUCCESS;
}
