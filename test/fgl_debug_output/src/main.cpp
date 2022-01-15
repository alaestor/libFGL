#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE
#include <cassert>
#include <sstream>
#include <string>

#include <fgl/debug/output.hpp>

#include "notmain.h"

#ifdef NDEBUG
	#error NDEBUG must not be defined for tests because they rely on assertions
#endif // NDEBUG

int main()
{
	using fgl::debug::output::config;
	assert(&config::instance().output_stream() == &std::cout);

	std::stringstream sstream;

	const auto last_output{
		[&sstream]()
		{
			const std::string s{ sstream.str() };
			sstream.clear(); // reset sstream state
			sstream.str(""); // empty stream
			return s;
		}
	};

	config::instance().change_output_stream(sstream);
	std::string maintest("main test");
	config::instance().output_stream() << maintest;
	assert(last_output() == maintest);
	notmain();
	assert(last_output() == std::string("notmain test"));

	return EXIT_SUCCESS;
}
