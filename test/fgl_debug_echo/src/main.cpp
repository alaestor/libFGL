#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE
#include <cassert>
#include <sstream>
#include <string_view>

#define FGL_DEBUG_ECHO_SHORT_MACROS
#include <fgl/debug/echo.hpp>

#include "notmain.hpp"

#ifdef NDEBUG
	#error NDEBUG must not be defined for tests because they rely on assertions
#endif // NDEBUG

std::string fixme_fmt_out(
	const long long int line,
	const std::string_view filePath,
	const std::string_view funcName,
	const std::string_view message)
{
	std::stringstream ss;
	ss << line << filePath << funcName << message;
	return ss.str();
}

std::string fixme_fmt(
	const fgl::debug::output::priority_e channel,
	const std::string_view message,
	const std::source_location source)
{
	assert(channel == fgl::debug::output::priority_e::echo);
	return fixme_fmt_out(
		source.line(),
		source.file_name(),
		source.function_name(),
		message
	);
}

std::string sfmt(
	const long long int line,
	const std::string_view filePath,
	const std::string_view funcName,
	const std::string_view message = "")
{
	return fixme_fmt_out(line, filePath, funcName, message);
}

std::string sfmt(
	const std::string_view message = "",
	const std::source_location source = std::source_location::current())
{
	return fixme_fmt(fgl::debug::output::priority_e::echo, message, source);
}

std::string result(const auto v)
{
	std::stringstream ss;
	ss << " == " << v << '\n';
	return ss.str();
}

int main()
{
	std::stringstream sstream;
	fgl::debug::output::config::instance().change_output_stream(sstream);
	fgl::debug::echo::config::instance().change_formatter(fixme_fmt);

	const auto last_output{
		[&sstream]()
		{
			const std::string s{ sstream.str() };
			sstream.clear(); // reset sstream state
			sstream.str(""); // empty stream
			return s;
		}
	};

	ECHO("test"); assert(last_output() == sfmt("test") + '\n');

	ECHOV(1+1); assert(last_output() == sfmt("1+1") + result(2));

	int x = 3;
	ECHOV(x); assert(last_output() == sfmt("x") + result(x));

	notmain();
	assert(
		last_output()
		== sfmt(12,"src/notmain.cpp","void notmain()","20") + result(20)
	);

	return EXIT_SUCCESS;
}
