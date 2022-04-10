#pragma once
#ifndef FGL_ENVIRCONMENT_COMPATIBILITY_CHECK_HPP_INCLUDED
#define FGL_ENVIRCONMENT_COMPATIBILITY_CHECK_HPP_INCLUDED

/// README
/*
This header is included by nearly everything in libFGL. It performs
preprocessor checks to ensure an *officially compatible* environment
(minimum C++ language specification and compiler version).

**The constraints on official compatibility are extremely narrow.** The true
viability of an environment will vary. At the time of writing, implementation
support for C++20 features is spotty at best; especially with regards to
`constexpr`. libFGL components and their tests make heavy use of `constexpr`.
Some tests may not compile or function properly within 'incompatible'
environments despite some of the components themselves being perfectly
viable in a runtime state. Users of libFGL may bypass compatibility checks
at their sole discretion.

**Bypassing checks:**

**All checks** can be bypassed by defining
`FGL_ACKNOWLEDGE_INCOMPATIBLE_ENVIRONMENT`

**Language version checks** can be bypassed by defining
`FGL_ACKNOWLEDGE_INCOMPATIBLE_STANDARD`

**Compiler version checks** can be bypassed by defining
`FGL_ACKNOWLEDGE_INCOMPATIBLE_COMPILER`
*/

#ifndef FGL_ACKNOWLEDGE_INCOMPATIBLE_ENVIRONMENT

// check standard version
#ifndef FGL_ACKNOWLEDGE_INCOMPATIBLE_STANDARD
	#if (__cplusplus < 202002L)
		#error "libFGL requires C++20 or higher. Define FGL_ACKNOWLEDGE_INCOMPATIBLE_STANDARD to bypass this check."
	#endif
#endif // FGL_ACKNOWLEDGE_INCOMPATIBLE_STANDARD

// check compiler
#ifndef FGL_ACKNOWLEDGE_INCOMPATIBLE_COMPILER
	#ifndef __GNUC__
		#error "libFGL is only officially supported on GCC 12.0.1 and higher. Define FGL_ACKNOWLEDGE_INCOMPATIBLE_COMPILER to bypass this check."
	#endif

	#if (__GNUC__ < 12)
	#if (__GNUC_MINOR__ < 1)
	#if (__GNUC_PATCHLEVEL__ < 1)
		#error "libFGL requires GCC 12.0.1 or higher. Define FGL_ACKNOWLEDGE_INCOMPATIBLE_COMPILER to bypass this check."
	#endif // patch
	#endif // minor
	#endif // major
#endif // FGL_ACKNOWLEDGE_INCOMPATIBLE_COMPILER

#endif // FGL_ACKNOWLEDGE_INCOMPATIBLE_ENVIRONMENT

#endif // FGL_ENVIRCONMENT_COMPATIBILITY_CHECK_HPP_INCLUDED
