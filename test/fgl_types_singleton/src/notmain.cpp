#include <cassert>

#include "notmain.h"
#include "single_class.hpp"

#ifdef NDEBUG
	#error NDEBUG must not be defined for tests because they rely on assertions
#endif // NDEBUG

void notmain()
{
	++Obj::instance().data;
}
