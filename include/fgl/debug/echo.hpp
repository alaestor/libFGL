#pragma once
#ifndef FGL_DEBUG_ECHO_HPP_INCLUDED
#define FGL_DEBUG_ECHO_HPP_INCLUDED

/// QUICK-START GUIDE
/*
	A simple way to output a string, or an expression with its result, to the
	debug output stream. Echo can be a nice replacement for "print debugging".

	FGL_DEBUG_ECHO("message"); // outputs "message"
	FGL_DEBUG_ECHOV(1+1); // ouputs "1+1 == 2"

	with #define FGL_DEBUG_CONSTEXPR_ASSERT_SHORT_MACROS or FGL_SHORT_MACROS

	ECHO("message");
	ECHOV(x);

	To disable all [ECHO] output, define NDEBUG above this include,
	or set fgl::debug::output::config threshold to be greater than echo
*/
/// EXAMPLE PROGRAM
/*
// define enables the short "ECHO" and "ECHOV"
// could also #define FGL_SHORT_MACROS
#define FGL_DEBUG_ECHO_SHORT_MACROS
#include <fgl/debug/echo.hpp>

auto add(const auto& a, const auto& b)
{
	const auto result = a + b;
	ECHOV(result);
	return result;
}

int main()
{
	ECHO("a message");
	ECHO("another message");
	ECHOV(add(1, 2));
}
*/
/// EXAMPLE OUTPUT
/*
[ECHO] file:src/main.cpp(18:2) 'int main()
 \_____ a message
[ECHO] file:src/main.cpp(19:2) 'int main()
 \_____ another message
[ECHO] file:src/main.cpp(9:2) 'int add(int, int)
 \_____ result == 3
[ECHO] file:src/main.cpp(20:2) 'int main()
 \_____ add(1, 2) == 3
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
		if (output::config::instance().above_priority_threshold(output::echo))
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
		if (output::config::instance().above_priority_threshold(output::echo))
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
