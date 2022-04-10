#pragma once
#ifndef FGL_DEBUG_EXCEPTION_OCCURS_HPP_INCLUDED
#define FGL_DEBUG_EXCEPTION_OCCURS_HPP_INCLUDED
#include "../environment/libfgl_compatibility_check.hpp"

/// QUICK-START GUIDE
/*
An immediate try-catch lambda wrapper which can be used to easily convert
exceptions to `bool`. Useful for testing and "inline" exception handling.

// assertions always true
assert(FGL_DEBUG_EXCEPTION_OCCURS( throw int ));
assert(FGL_DEBUG_EXCEPTION_OCCURS( func_always_throws() ));
assert(!FGL_DEBUG_EXCEPTION_OCCURS( func_never_throws() ));
assert(!FGL_DEBUG_SPECIFIC_EXCEPTION_OCCURS( std::exception, func_never_throws() ));
assert(FGL_DEBUG_SPECIFIC_EXCEPTION_OCCURS( expected_e, func_throws_expected_e() ));
assert(!FGL_DEBUG_SPECIFIC_EXCEPTION_OCCURS( different_e, func_throws_expected_e() ));

// if you #define FGL_DEBUG_EXCEPTION_OCCURS_SHORT_MACROS or FGL_SHORT_MACROS

assert(exception_occurs( throw int ));
assert(exception_occurs( func_always_throws() ));
assert(!exception_occurs( func_never_throws() ));
assert(!specific_exception_occurs( std::exception, func_never_throws() ));
assert(specific_exception_occurs( expected_e, func_throws_expected_e() ));

// note: `excpected_e` exception isn't caught and will propogate.
try{ specific_exception_occurs( different_e, func_throws_expected_e() ); }
catch(expected_e&)
{
	// we get here
}

// example of dealing with a stupid_lib::Thing factory
stupid_lib::Thing thing;

while (specific_exception_occurs(
	stupid_lib::busy,
	thing = stupid_lib::make_thing()))
{
	wait_a_bit();
}

use(thing);


// A NOTE ABOUT ERROR-HANDLING DESIGN

It's worth noting that the idea of using exception-to-bool conversions for
error handling is bad. Aside from making exception tests easier to read,
this should only be employed in niche circumstances and not used as an
error-handling design strategy. Exceptions should be exceptional, but some
libraries are stupid and this can help contain their stupidity in a
relatively clean and readable way.
*/

#ifdef FGL_SHORT_MACROS
	#define FGL_DEBUG_EXCEPTION_OCCURS_SHORT_MACROS
#endif // ifdef FGL_SIMPLE_MACROS

#ifndef FGL_DEBUG_EXCEPTION_OCCURS
///////////////////////////////////////////////////////////////////////////////
	#define FGL_DEBUG_EXCEPTION_OCCURS(expression) \
		([&]() -> bool {\
		try{ expression; return false; }\
		catch(...){ return true; }}())
///////////////////////////////////////////////////////////////////////////////
#else
	#error FGL_DEBUG_EXCEPTION_OCCURS already defined
#endif // ifndef FGL_DEBUG_EXCEPTION_OCCURS

#ifndef FGL_DEBUG_SPECIFIC_EXCEPTION_OCCURS
///////////////////////////////////////////////////////////////////////////////
	#define FGL_DEBUG_SPECIFIC_EXCEPTION_OCCURS(exception, expression) \
		([&]() -> bool {\
		try{ expression; return false; }\
		catch(const exception &){ return true; }\
		catch(...){ throw; }}())
///////////////////////////////////////////////////////////////////////////////
#else
	#error FGL_DEBUG_SPECIFIC_EXCEPTION_OCCURS already defined
#endif // ifndef FGL_DEBUG_SPECIFIC_EXCEPTION_OCCURS

// Opt-in short macros to avoid collisions
#ifdef FGL_DEBUG_EXCEPTION_OCCURS_SHORT_MACROS
	#ifndef exception_occurs
///////////////////////////////////////////////////////////////////////////////
		#define exception_occurs(expression) \
			FGL_DEBUG_EXCEPTION_OCCURS(expression)
///////////////////////////////////////////////////////////////////////////////
	#else
		#error exception_occurs already defined
	#endif // exception_occurs
	#ifndef specific_exception_occurs
///////////////////////////////////////////////////////////////////////////////
		#define specific_exception_occurs(exception, expression) \
			FGL_DEBUG_SPECIFIC_EXCEPTION_OCCURS(exception, expression)
///////////////////////////////////////////////////////////////////////////////
	#else
		#error specific_exception_occurs already defined
	#endif // specific_exception_occurs
#endif // ifdef FGL_DEBUG_EXCEPTION_OCCURS_SHORT_MACROS

#endif // FGL_DEBUG_EXCEPTION_OCCURS_HPP_INCLUDED
