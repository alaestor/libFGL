#pragma once
#ifndef FGL_DEBUG_CONSTEXPR_ASSERT_HPP_INCLUDED
#define FGL_DEBUG_CONSTEXPR_ASSERT_HPP_INCLUDED

/*
	Trying to figure out why constexpr_assert is not defined?
	Make sure to define #FGL_SHORT_MACROS before including any libFGL headers!
*/

/// QUICK-START GUIDE
/*
	An assert-like macro valid for both compile-time and runtime assertions

	FGL_DEBUG_CONSTEXPR_ASSERT( boolean expression );

	To get the short version, define: FGL_SHORT_MACROS (global FGL macro)
	or specific to this file: FGL_DEBUG_CONSTEXPR_ASSERT_SHORT_MACROS

	constexpr_assert( boolean expression  );

	To disable all assertions, define NDEBUG above this include.
*/
///EXAMPLE PROGRAM
/*
	#define FGL_SHORT_MACROS
	#include <fgl/debug/constexpr_assert.hpp>

	// can be executed at both compile-time and run-time
	constexpr int add_and_assert_two(int a, int b)
	{
		const int result{ a + b };
		constexpr_assert(result == 2);
		return result;
	}

	int main()
	{
		constexpr int a{ add_and_assert_two(1, 1) }; // compile-time, OK
		constexpr int b{ add_and_assert_two(5, 5) }; // compile-time, ERROR
		int c{ add_and_assert_two(1, 1) }; // runtime, OK
		int d{ add_and_assert_two(5, 5) }; // runtime, ERROR
	}
*/
/// README (the long version)
/*
	`FGL_DEBUG_CONSTEXPR_ASSERT()` / `constexpr_assert()` are macros similar to
	assert. The boolean condition doesn't need to be a constant expression. If
	the statement executes at runtime then the regular C `assert` will be used.
	If the statement executes at compile-time and the assertion is false,
	the `constexpr_assert` will throw causing an error; compilation will fail.

	This header guarentees not redefine any existing macros (will #error)

	To disable all `constexpr_assert` macros, define NDEBUG above this include.

	To use short macros, define FGL_DEBUG_CONSTEXPR_ASSERT_SHORT_MACROS
		above this include. Short macros (`constexpr_assert`) are opt-in to
		avoid collisions. The long names are prefixed with "FGL_DEBUG_".
		Alternatively, define FGL_SHORT_MACROS (effects all FGL header macros).
*/

#include <type_traits> // is_constant_evaluated
#include <cassert>

#ifdef FGL_SHORT_MACROS
	#define FGL_DEBUG_CONSTEXPR_ASSERT_SHORT_MACROS
#endif // ifdef FGL_SIMPLE_MACROS

/* TODO -> future reflection overhaul
	Complete overhaul when C++ gets reflection and compile-time output.
	Minimize macro usage.
*/

#ifndef NDEBUG

	#ifndef FGL_DEBUG_CONSTEXPR_ASSERT
///////////////////////////////////////////////////////////////////////////////
		#define FGL_DEBUG_CONSTEXPR_ASSERT(assertion) \
			if (std::is_constant_evaluated())\
			{\
				if ((assertion) == false) throw;\
			}\
			else assert(assertion);
///////////////////////////////////////////////////////////////////////////////
	#else
		#error FGL_DEBUG_CONSTEXPR_ASSERT already defined
	#endif // ifndef FGL_DEBUG_CONSTEXPR_ASSERT

	// Opt-in short macros to avoid collisions
	#ifdef FGL_DEBUG_CONSTEXPR_ASSERT_SHORT_MACROS
		#ifndef constexpr_assert
///////////////////////////////////////////////////////////////////////////////
			#define constexpr_assert(assertion) \
				FGL_DEBUG_CONSTEXPR_ASSERT(assertion)
///////////////////////////////////////////////////////////////////////////////
		#else
			#error constexpr_assert already defined
		#endif // constexpr_assert
	#endif // ifdef FGL_DEBUG_CONSTEXPR_ASSERT_SHORT_MACROS

#else

	#ifndef FGL_DEBUG_CONSTEXPR_ASSERT
///////////////////////////////////////////////////////////////////////////////
    	#define FGL_DEBUG_CONSTEXPR_ASSERT(assertion)
///////////////////////////////////////////////////////////////////////////////
	#else
		#error FGL_DEBUG_CONSTEXPR_ASSERT already defined
	#endif // ifndef FGL_DEBUG_CONSTEXPR_ASSERT

	#ifdef FGL_DEBUG_CONSTEXPR_ASSERT_SHORT_MACROS
		#ifndef constexpr_assert
///////////////////////////////////////////////////////////////////////////////
			#define constexpr_assert(assertion)
///////////////////////////////////////////////////////////////////////////////
		#else
			#error constexpr_assert already defined
		#endif // ifndef constexpr_assert
	#endif

#endif // ifndef NDEBUG

#endif // ifndef FGL_DEBUG_CONSTEXPR_ASSERT_HPP_INCLUDED
