#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE
#include <cassert>
#include <sstream>
#include <string_view>

#define FGL_DEBUG_FIXME_SHORT_MACROS
#include <fgl/debug/fixme.hpp>

#include "notmain.h"

#ifdef NDEBUG
	#error NDEBUG must not be defined for tests because they rely on assertions
#endif // NDEBUG

void fixme_fmt_out(
	std::ostream& output_stream,
	const long long int line,
	const std::string_view filePath,
	const std::string_view funcName,
	const std::string_view message)
{
	output_stream << line << filePath << funcName << message;
}

void fixme_fmt(
	std::ostream& output_stream,
	const std::string_view message,
	const std::source_location source)
{
	fixme_fmt_out(
		output_stream,
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
	std::stringstream s;
	fixme_fmt_out(s, line, filePath, funcName, message);
	return s.str();
}

std::string sfmt(
	const std::string_view message = "",
	const std::source_location source = std::source_location::current())
{
	std::stringstream s;
	fixme_fmt(s, message, source);
	return s.str();
}

int main()
{
	std::stringstream sstream;
	fgl::debug::output::config::instance().change_output_stream(sstream);
	fgl::debug::fixme::config::instance().change_formatter(fixme_fmt);

	const auto last_output{
		[&sstream]()
		{
			const std::string s{ sstream.str() };
			sstream.clear(); // reset sstream state
			sstream.str(""); // empty stream
			return s;
		}
	};

	FIX_ME; assert(last_output() == sfmt());

	FIX_ME static_assert(true); assert(last_output() == sfmt());

	FIX("a message"); assert(last_output() == sfmt("a message"));

	FIX_THIS( [[maybe_unused]] int five(2+2); )
	assert(last_output() == sfmt(__LINE__-1,__FILE__,"int main()",
		"[[maybe_unused]] int five(2+2);"));

	notmain();
	assert(last_output() == sfmt(12,"src/notmain.cpp","void notmain()","test"));

	return EXIT_SUCCESS;
}
