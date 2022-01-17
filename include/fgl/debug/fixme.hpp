#pragma once
#ifndef FGL_DEBUG_FIXME_HPP_INCLUDED
#define FGL_DEBUG_FIXME_HPP_INCLUDED

/// QUICK-START GUIDE
/*
	FGL_DEBUG_FIX_ME; // simple output
	FGL_DEBUG_FIX("a message"); // providing a message
	FGL_DEBUG_FIX_THIS(code); // message is code (which is still executed)

	To get the short version, define: FGL_SHORT_MACROS (global FGL macro)
	or specific to this file: FGL_DEBUG_FIXME_SHORT_MACROS

	FIX_ME;
	FIX("a message");
	FIX_THIS( code );

	To disable all [FIXME] output, define NDEBUG above this include.

	[FIXME] provides a simple macro interface to leave reminders, notes,
	code review tags, and document things that are a work-in-progress.
	Sends formatted output to the FGL debug output stream
	(std::cout by default, configurable). See example or fgl/debug/output.hpp
*/
/// EXAMPLE PROGRAM
/*
	#include <iostream>
	#define FGL_SHORT_MACROS
	#include <fgl/debug/fixme.hpp>

	// can be executed at both compile-time and run-time
	int add(int a, int b)
	{
		FIX_THIS( return a - b; )
	}

	int main()
	{
		// redirect global debug output to cerr instead of cout (default)
		fgl::debug::output::Config::instance().change_output_stream(std::cerr);

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
/// README (the long version)
/*
	[FIXME] provides a simple macro interface to leave reminders, notes,
	code review tags, and document things that are a work-in-progress.

	This header guarentees not redefine any existing macros (will #error)

	To disable all [FIXME] output, define NDEBUG above this include.

	To use short macros, define FGL_DEBUG_FIXME_SHORT_MACROS above this include.
		Short macros (`FIX`, `FIX_ME`, `FIX_THIS`) are opt-in to avoid
		collisions. The long names are prefixed with "FGL_DEBUG_".
		Alternatively, define FGL_SHORT_MACROS (affects all FGL header macros).

	To change the output string format, implement a function and assign to
		`fgl::debug::fixme::Config::getInstance().formatter = my_formatter;`
		The default is `fgl::debug::fixme::Config::default_formatter`

	To change where fgl::debug output is sent, provide an ostream to
		fgl::debug::Config::getInstance().change_output_stream( my_ostream );
		The default is `std::cout`.
*/

/* TODO -> future module overhaul
	Will still need a .h for macros
*/

/* TODO -> future reflection overhaul
	Complete overhaul when C++ gets reflection and compile-time output.
	Separate compiletime and runtime FIXME interfaces. Minimize macro usage.
	Lowest macro will expand to constexpr{reflection}? Macro may not be needed.
*/

#include <functional>
#include <ostream>
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
	#define FGL_DEBUG_FIX_THIS(code) code
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
			#define FGL_DEBUG_FIX_THIS_IMPL(code) FGL_DEBUG_FIX(#code) code;
///////////////////////////////////////////////////////////////////////////////
		#else
			#error FGL_DEBUG_FIX_THIS_IMPL already defined
		#endif // ifndef FGL_DEBUG_FIX_THIS_IMPL
///////////////////////////////////////////////////////////////////////////////
		#define FGL_DEBUG_FIX_THIS(code) FGL_DEBUG_FIX_THIS_IMPL(code)
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
		#define FIX_THIS(code) FGL_DEBUG_FIX_THIS_IMPL(code)
///////////////////////////////////////////////////////////////////////////////
	#else
		#error FIX_THIS already defined (FGL_DEBUG_FIXME_SHORT_MACROS)
	#endif // ifndef FIX_THIS
#endif // ifdef FGL_DEBUG_FIXME_SHORT_MACROS

namespace fgl::debug::fixme {

namespace internal {
class config final
{
public:
	static void default_formatter(
		std::ostream& output_stream,
		const std::string_view message,
		const std::source_location source)
	{ // TODO constexpr when std::string::operator+ is implemented?
		output_stream
			<< "[FIXME] file:" << source.file_name()
			<< '(' << source.line() << ':' << source.column() << ") '"
			<< source.function_name() << '\''
			<< (message.empty() ? "" : "\n \\_____ ") << message << '\n';
	}
	using fixme_formatter_t = std::function<decltype(default_formatter)>;

	const fixme_formatter_t& formatter{ m_formatter };

	void change_formatter(const fixme_formatter_t& custom_formatter) noexcept
	{ m_formatter = custom_formatter; }

	void emit_fixme_message(
		const std::string_view message = "",
		const std::source_location source
			= std::source_location::current()) const
	{
		auto& ostream{ fgl::debug::output::config::instance().output_stream() };
		m_formatter(ostream, message, source );
	}

private:
	FGL_SINGLETON_BOILERPLATE(config);
	fixme_formatter_t m_formatter{ default_formatter };
};
}// namespace internal

using config = fgl::singleton<internal::config>;

}// namespace fgl::debug::fixme

#endif // ifndef FGL_DEBUG_FIXME_HPP_INCLUDED
