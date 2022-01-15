#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE
#include <cassert>

#include "forward/iterator.hpp"
#include "forward/range.hpp"

#define FGL_SHORT_MACROS
#include <fgl/debug./constexpr_assert.hpp>

#ifdef NDEBUG
	#error NDEBUG must not be defined for tests because they rely on assertions
#endif // NDEBUG

int main()
{
	static_assert(forward::iterator::tests());
	constexpr_assert(forward::range::tests());
	return EXIT_SUCCESS;
}
