#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE
#include <cassert>
#include <sstream>
#include <string_view>

#define FGL_DEBUG_FIXME_SHORT_MACROS
#include <fgl/debug/fixme.hpp>

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
	assert(channel == fgl::debug::output::priority_e::fixme);
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
	return fixme_fmt_out(line, filePath, funcName, message) + '\n';
}

std::string sfmt(
	const std::string_view message = "",
	const std::source_location source = std::source_location::current())
{
	return
		fixme_fmt(fgl::debug::output::priority_e::fixme, message, source)
		+ '\n';
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
