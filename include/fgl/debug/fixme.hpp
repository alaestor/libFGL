#pragma once
#ifndef FGL_DEBUG_FIXME_HPP_INCLUDED
#define FGL_DEBUG_FIXME_HPP_INCLUDED

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
	or set fgl::debug::output::config threshold to be greater than fixme
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

#include "../types/singleton.hpp"
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
			fgl::debug::fixme::config::instance().emit_fixme_message(message);
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

namespace fgl::debug::fixme {

using fixme_formatter_t = output::default_formatter_t;

namespace internal {
class config final
{
	FGL_SINGLETON_BOILERPLATE(config);

public:
	const fixme_formatter_t& formatter{ m_formatter };

	void change_formatter(const fixme_formatter_t& custom_formatter) noexcept
	{ m_formatter = custom_formatter; }

	void emit_fixme_message(
		const std::string_view message = "",
		const std::source_location source
			= std::source_location::current()) const
	{
		if (output::config::instance().above_priority_threshold(output::fixme))
		{
			output::config::instance().output_stream()
				<< m_formatter(output::fixme, message, source)
				<< '\n';
		}
	}

private:
	fixme_formatter_t m_formatter{
		output::internal::config::default_formatter
	};
};
}// namespace internal

using config = fgl::singleton<internal::config>;

}// namespace fgl::debug::fixme

#endif // ifndef FGL_DEBUG_FIXME_HPP_INCLUDED
