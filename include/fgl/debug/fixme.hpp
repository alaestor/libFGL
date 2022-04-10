#pragma once
#ifndef FGL_DEBUG_FIXME_HPP_INCLUDED
#define FGL_DEBUG_FIXME_HPP_INCLUDED
#include "../environment/libfgl_compatibility_check.hpp"

/// QUICK-START GUIDE
/*
	A simple way to leave reminders, notes, code review tags,
	and easily document things that are a work-in-progress.

	FGL_DEBUG_FIX_ME; // simple output
	FGL_DEBUG_FIX("a message"); // providing a message
	FGL_DEBUG_FIX_THIS(expression); // message is code (will be executed)

	with #define FGL_DEBUG_FIXME_SHORT_MACROS or FGL_SHORT_MACROS

	FIX_ME;
	FIX("a message");
	FIX_THIS(expression);

	To disable all [FIXME] output, define NDEBUG above this include,
	or call fgl::debug::output_config<fgl::debug::fixme>::turn_off();
*/
/// EXAMPLE PROGRAM
/*
#include <iostream>

// define enables the short "FIX", "FIX_ME", and "FIX_THIS"
// could also #define FGL_SHORT_MACROS
#define FGL_DEBUG_FIXME_SHORT_MACROS
#include <fgl/debug/fixme.hpp>

int add(int a, int b)
{
	FIX_THIS( return a - b; )
}

int main()
{
	FIX_ME;
	FIX_ME const int one{ 1};
	const int two{ 2 }; FIX_ME
	const int three{ add(one, two) };
	std::cout << one << " + " << two << " = " << three << std::endl;
	FIX("the author is an idiot")
}
*/
/// EXAMPLE OUTPUT
/*
[FIXME] file:src/main.cpp(15:3) 'int main()'
[FIXME] file:src/main.cpp(16:3) 'int main()'
[FIXME] file:src/main.cpp(17:23) 'int main()'
[FIXME] file:src/main.cpp(8:3) 'int add(int, int)'
 \_____ return a - b;
1 + 2 = -1
[FIXME] file:src/main.cpp(20:3) 'int main()'
 \_____ the author is an idiot
*/



/// INTERNAL NOTES
/* TODO -> future module overhaul
	Will still need a .h for macros
*/
/* TODO -> future reflection overhaul
	Complete overhaul when C++ gets reflection and compile-time output.
	Separate compiletime and runtime FIXME interfaces. Minimize macro usage.
	Lowest macro will expand to constexpr{reflection}? Macro may not be needed.
*/

#include <string_view>
#include <source_location>

#include "../debug/output.hpp"

#ifdef FGL_SHORT_MACROS
	#define FGL_DEBUG_FIXME_SHORT_MACROS
#endif // FGL_SHORT_MACROS

#ifdef NDEBUG
	#define FGL_DEBUG_FIX(message)
	#define FGL_DEBUG_FIX_ME
	#define FGL_DEBUG_FIX_THIS(expr) expr
#else
	#ifndef FGL_DEBUG_FIX
///////////////////////////////////////////////////////////////////////////////
		#define FGL_DEBUG_FIX(message) \
			fgl::debug::output(fgl::debug::fixme(message));
///////////////////////////////////////////////////////////////////////////////
	#else
		#error FGL_DEBUG_FIX already defined
	#endif // ifndef FGL_DEBUG_FIX

	#ifndef FGL_DEBUG_FIX_ME
///////////////////////////////////////////////////////////////////////////////
		#define FGL_DEBUG_FIX_ME FGL_DEBUG_FIX("")
///////////////////////////////////////////////////////////////////////////////
	#else
		#error FGL_DEBUG_FIX_ME already defined
	#endif // ifndef FGL_DEBUG_FIX_ME

	#ifndef FGL_DEBUG_FIX_THIS
		#ifndef FGL_DEBUG_FIX_THIS_IMPL
///////////////////////////////////////////////////////////////////////////////
			#define FGL_DEBUG_FIX_THIS_IMPL(expr) FGL_DEBUG_FIX(#expr) expr;
///////////////////////////////////////////////////////////////////////////////
		#else
			#error FGL_DEBUG_FIX_THIS_IMPL already defined
		#endif // ifndef FGL_DEBUG_FIX_THIS_IMPL
///////////////////////////////////////////////////////////////////////////////
		#define FGL_DEBUG_FIX_THIS(expr) FGL_DEBUG_FIX_THIS_IMPL(expr)
///////////////////////////////////////////////////////////////////////////////
	#else
		#error FGL_DEBUG_FIX_THIS already defined
	#endif // ifndef FGL_DEBUG_FIX_THIS
#endif // ifndef NDEBUG

// Opt-in short macros to avoid collisions
#ifdef FGL_DEBUG_FIXME_SHORT_MACROS
	#ifndef FIX
///////////////////////////////////////////////////////////////////////////////
		#define FIX(message) FGL_DEBUG_FIX(message)
///////////////////////////////////////////////////////////////////////////////
	#else
		#error FIX already defined (FGL_DEBUG_FIXME_SHORT_MACROS)
	#endif // ifndef FIX

	#ifndef FIX_ME
///////////////////////////////////////////////////////////////////////////////
		#define FIX_ME FGL_DEBUG_FIX_ME
///////////////////////////////////////////////////////////////////////////////
	#else
		#error FIX_ME already defined (FGL_DEBUG_FIXME_SHORT_MACROS)
	#endif // ifndef FIX_ME

	#ifndef FIX_THIS
///////////////////////////////////////////////////////////////////////////////
		#define FIX_THIS(expr) FGL_DEBUG_FIX_THIS_IMPL(expr)
///////////////////////////////////////////////////////////////////////////////
	#else
		#error FIX_THIS already defined (FGL_DEBUG_FIXME_SHORT_MACROS)
	#endif // ifndef FIX_THIS
#endif // ifdef FGL_DEBUG_FIXME_SHORT_MACROS

namespace fgl::debug {

struct fixme final
{
	const std::string_view message;
	const std::source_location location;

	[[nodiscard]] constexpr explicit
	fixme(
		const std::string_view msg,
		const std::source_location& loc = std::source_location::current())
	: message{ msg }, location{ loc }
	{}
};

template <>
class output_config<fixme> final
{
	static inline bool m_enabled{ true };
	public:
	output_config(auto&&...) = delete;
	~output_config() = delete;

	static void turn_on() { m_enabled = true; }
	static void turn_off() { m_enabled = false; }
	static bool enabled() { return m_enabled; }
	static priority get_priority() { return priority::debug; }
	static std::string_view name() { return "FIXME"; }

	static inline output::format_msg_src_t formatter{
		output::default_fmt_msg_src
	};

	[[nodiscard]] static std::string format(const fixme& fixme)
	{
		return formatter(fixme.message, fixme.location);
	}
};

static_assert(output_handler<output_config<fixme>, fixme>);

} // namespace fgl::debug

#endif // ifndef FGL_DEBUG_FIXME_HPP_INCLUDED
