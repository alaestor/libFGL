#pragma once
#ifndef FGL_DEBUG_ECHO_HPP_INCLUDED
#define FGL_DEBUG_ECHO_HPP_INCLUDED
#include "../environment/libfgl_compatibility_check.hpp"

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
	or call fgl::debug::output_config<fgl::debug::echo>::turn_off();
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
#include <sstream>
#include <source_location>

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
			fgl::debug::output_config<fgl::debug::echo>::emit(message);
///////////////////////////////////////////////////////////////////////////////
	#else
		#error FGL_DEBUG_ECHO already defined
	#endif // ifndef FGL_DEBUG_ECHO

	#ifndef FGL_DEBUG_ECHOV
		#ifndef FGL_DEBUG_ECHOV_IMPL
///////////////////////////////////////////////////////////////////////////////
			#define FGL_DEBUG_ECHOV_IMPL(expr) \
				fgl::debug::output_config<fgl::debug::echo>::emit(expr, #expr);
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
		#define ECHOV(expr) FGL_DEBUG_ECHOV(expr)
///////////////////////////////////////////////////////////////////////////////
	#else
		#error ECHOV already defined (FGL_DEBUG_ECHO_SHORT_MACROS)
	#endif // ifndef ECHOV

#endif // ifdef FGL_DEBUG_ECHO_SHORT_MACROS

namespace fgl::debug {

class echo final
{
	public:
	echo(auto&&...) = delete;
	~echo() = delete;
};

template <>
class output_config<echo> final
{
	static inline bool m_enabled{ true };
	public:
	output_config(auto&&...) = delete;
	~output_config() = delete;

	static void turn_on() { m_enabled = true; }
	static void turn_off() { m_enabled = false; }
	static bool enabled() { return m_enabled; }
	static priority get_priority() { return priority::debug; }
	static std::string_view name() { return "ECHO"; }

	template <typename T>
	[[nodiscard]] static std::string default_fmt_value(const T& value)
	{
		std::stringstream ss;
		ss << " == " << value;
		return ss.str();
	}

	template <typename T>
	static inline std::function<decltype(default_fmt_value<T>)> format_value{
		default_fmt_value<T>
	};

	static inline output::format_msg_src_t formatter {
		output::default_fmt_msg_src
	};

	static void emit(
		const std::string_view message,
		const std::source_location source = std::source_location::current())
	{
		if (const auto& o{ output::channel_stream<output_config<echo>>() }; o)
		{
			o.value().get()
				<< output::format_head(name())
				<< formatter(message, source)
				<< '\n';
		}
	}

	static void emit(
		const auto result,
		const std::string_view message,
		const std::source_location source = std::source_location::current())
	{
		if (const auto& o{ output::channel_stream<output_config<echo>>() }; o)
		{
			o.value().get()
				<< output::format_head(name())
				<< formatter(message, source)
				<< format_value<decltype(result)>(result)
				<< '\n';
		}
	}
};

static_assert(output_channel<output_config<echo>>);

}// namespace fgl::debug

#endif // ifndef FGL_DEBUG_ECHO_HPP_INCLUDED
