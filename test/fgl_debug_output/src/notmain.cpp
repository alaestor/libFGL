#include <fgl/debug/output.hpp>

#include "notmain.h"

#ifdef NDEBUG
	#error NDEBUG must not be defined for tests because they rely on assertions
#endif // NDEBUG

void notmain()
{
	fgl::debug::output::config::instance().output_stream() << "notmain test";
}
