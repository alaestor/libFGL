#pragma once
#ifndef FGL_DEBUG_ECHO_HPP_INCLUDED
#define FGL_DEBUG_ECHO_HPP_INCLUDED

/// QUICK-START GUIDE
/*
	FGL_DEBUG_ECHO("message"); // simple message
	FGL_DEBUG_ECHOV(1+1); // ouputs the expression and it's value

	For short macros, define FGL_SHORT_MACROS (for all libFGL headers)
	or specific to this file: FGL_DEBUG_ECHO_SHORT_MACROS

	ECHO("message");
	ECHOV(x);

	To disable all [ECHO] output, define NDEBUG above this include.
	Or, set fgl::debug::output threshold to > echo
*/
/// EXAMPLE PROGRAM
/*
#include <iostream>
#define FGL_SHORT_MACROS
#include <fgl/debug/echo.hpp>

int add(int a, int b)
{
	auto result = a+b;
	ECHOV(result);
	return result;
}

int main()
{
	// redirect global debug output to cerr instead of cout (default)
	fgl::debug::output::config::instance().change_output_stream(std::cerr);

	ECHO("a message");
	ECHO("another message");

	const int three{ add(1, 2) };
	ECHOV(three);
}
*/
/// EXAMPLE OUTPUT
/*
[ECHO] file:src/main.cpp(16:3) 'int main()
 \_____ a message
[ECHO] file:src/main.cpp(17:3) 'int main()
 \_____ another message
[ECHO] file:src/main.cpp(7:3) 'int add(int, int)
 \_____ result == 3
[ECHO] file:src/main.cpp(20:3) 'int main()
 \_____ three == 3
*/
/// README (the long version)
/*
	[ECHO] provides a simple macro interface to print messages and values to
	libFGL's debug output stream.

	ECHO("message"); will output a simple message with location info.
	ECHOV(expression); will output the expression and it's value.

	This header guarentees not redefine any existing macros (will #error)

	To disable all [ECHO] output, define NDEBUG above this include.
	Or, set fgl::debug::output threshold to > echo.

	To use short macros, define FGL_DEBUG_ECHO_SHORT_MACROS above this include.
		Short macros (`ECHO`, `ECHOV`) are opt-in to avoid collisions. The
		long names are prefixed with "FGL_DEBUG_".
		Alternatively, define FGL_SHORT_MACROS (affects all FGL header macros).

	To change the output string format, implement a function and assign to
		`fgl::debug::echo::config::getInstance().formatter = my_formatter;`
		The default is `fgl::debug::output::config::default_formatter`
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
	#define FGL_DEBUG_ECHO_SHORT_MACROS
#endif // FGL_SHORT_MACROS

#ifdef NDEBUG
	#define FGL_DEBUG_ECHO(message)
	#define FGL_DEBUG_ECHOV(expr) expr
#else
	#ifndef FGL_DEBUG_ECHO
///////////////////////////////////////////////////////////////////////////////
		#define FGL_DEBUG_ECHO(message) \
			fgl::debug::echo::config::instance().emit_echo(message);
///////////////////////////////////////////////////////////////////////////////
	#else
		#error FGL_DEBUG_ECHO already defined
	#endif // ifndef FGL_DEBUG_ECHO

	#ifndef FGL_DEBUG_ECHOV
		#ifndef FGL_DEBUG_ECHOV_IMPL
///////////////////////////////////////////////////////////////////////////////
			#define FGL_DEBUG_ECHOV_IMPL(expr) \
				fgl::debug::echo::config::instance().emit_echo(expr, #expr);
///////////////////////////////////////////////////////////////////////////////
		#else
			#error FGL_DEBUG_ECHOV_IMPL already defined
		#endif // ifndef FGL_DEBUG_ECHOV_IMPL
///////////////////////////////////////////////////////////////////////////////
		#define FGL_DEBUG_ECHOV(expr) FGL_DEBUG_ECHOV_IMPL(expr)
///////////////////////////////////////////////////////////////////////////////
	#else
		#error FGL_DEBUG_ECHOV already defined
	#endif // ifndef FGL_DEBUG_ECHOV

#endif // ifndef NDEBUG

// Opt-in short macros to avoid collisions
#ifdef FGL_DEBUG_ECHO_SHORT_MACROS

	#ifndef ECHO
///////////////////////////////////////////////////////////////////////////////
		#define ECHO(message) FGL_DEBUG_ECHO(message)
///////////////////////////////////////////////////////////////////////////////
	#else
		#error ECHO already defined (FGL_DEBUG_ECHO_SHORT_MACROS)
	#endif // ifndef ECHO

	#ifndef ECHOV
///////////////////////////////////////////////////////////////////////////////
		#define ECHOV(expr) FGL_DEBUG_ECHOV_IMPL(expr)
///////////////////////////////////////////////////////////////////////////////
	#else
		#error ECHOV already defined (FGL_DEBUG_ECHO_SHORT_MACROS)
	#endif // ifndef ECHOV

#endif // ifdef FGL_DEBUG_ECHO_SHORT_MACROS

namespace fgl::debug::echo {

using echo_formatter_t = output::default_formatter_t;

namespace internal {
class config final
{
	FGL_SINGLETON_BOILERPLATE(config);
public:
	const echo_formatter_t& formatter{ m_formatter };

	void change_formatter(const echo_formatter_t& custom_formatter) noexcept
	{ m_formatter = custom_formatter; }

	void emit_echo(
		const std::string_view message,
		const std::source_location source
			= std::source_location::current()) const
	{
		if (output::config::instance().channel_is_enabled(output::echo))
		{
			output::config::instance().output_stream()
				<< m_formatter(output::echo, message, source) << '\n';
		}
	}

	void emit_echo(
		const auto result,
		const std::string_view message,
		const std::source_location source
			= std::source_location::current()) const
	{
		if (output::config::instance().channel_is_enabled(output::echo))
		{
			output::config::instance().output_stream()
				<< m_formatter(output::echo, message, source)
				<< " == " << result << '\n';
		}
	}

private:
	echo_formatter_t m_formatter{
		output::internal::config::default_formatter
	};
};
}// namespace internal

using config = fgl::singleton<internal::config>;

}// namespace fgl::debug::echo

#endif // ifndef FGL_DEBUG_ECHO_HPP_INCLUDED
