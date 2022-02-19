#pragma once
#ifndef FGL_DEBUG_CONSTEXPR_ASSERT_HPP_INCLUDED
#define FGL_DEBUG_CONSTEXPR_ASSERT_HPP_INCLUDED

/*
	Trying to figure out why constexpr_assert is not defined?
	Make sure FGL_SHORT_MACROS or FGL_DEBUG_CONSTEXPR_ASSERT_SHORT_MACROS is
	defined before including any libFGL headers!
*/

/// QUICK-START GUIDE
/*
	An assert-like macro with well-defined behavior for compile-time
	(`constexpr` / `consteval`) evaluation.

	FGL_DEBUG_CONSTEXPR_ASSERT(1 + 1 == 2);
	FGL_DEBUG_CONSTEXPR_ASSERT(1 + 1 == 3); // FATAL ERROR

	with #define FGL_DEBUG_CONSTEXPR_ASSERT_SHORT_MACROS or FGL_SHORT_MACROS

	constexpr_assert(2 + 2 == 4);
	constexpr_assert(2 + 2 == 5); // FATAL ERROR
*/
///EXAMPLE PROGRAM
/*
	// define enables the short "constexpr_assert"
	// could also use #define FGL_SHORT_MACROS
	#define FGL_DEBUG_CONSTEXPR_ASSERT_SHORT_MACROS
	#include <fgl/debug/constexpr_assert.hpp>

	// add two integers and assert that the result is 2
	constexpr int add_and_assert_two(int a, int b)
	{
		const int result{ a + b };
		constexpr_assert(result == 2);
		// without short macros, use:
		// FGL_DEBUG_CONSTEXPR_ASSERT(result == 2);
		return result;
	}

	int main()
	{
		constexpr int a{ add_and_assert_two(1, 1) }; // OK
		constexpr int b{ add_and_assert_two(5, 5) }; // COMPILE-TIME ERROR
		int c{ add_and_assert_two(1, 1) }; // OK
		int d{ add_and_assert_two(5, 5) }; // RUNTIME ERROR (via assert)
	}
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
