#pragma once
#ifndef FGL_DEBUG_OUTPUT_HPP_INCLUDED
#define FGL_DEBUG_OUTPUT_HPP_INCLUDED

/// QUICK-START GUIDE / EXAMPLE PROGRAM
/*
	#include <iostream>

	// this usually wouldn't be used on it's own;
	// it's meant to direct the output stream of fgl::debug tools
	#include <fgl/debug/output.hpp>

	int main()
	{
		// redirect global debug output to cerr instead of cout (default)
		fgl::debug::output::Config::instance().change_output_stream(std::cerr);
		// example of something sending input to the stream
		fgl::debug::output::Config::instance().output_stream() << "derp";
	}
*/
/// EXAMPLE OUTPUT
/*
derp
*/

#include <cassert>
#include <iostream>

#include "../types/singleton.hpp"

namespace fgl::debug::output {

namespace internal {
class config final
{
	FGL_SINGLETON_BOILERPLATE(config);

	std::ostream* m_output_stream{ &std::cout };

public:
	void change_output_stream(std::ostream* const output_stream) noexcept
	{
		assert(output_stream != nullptr);
		m_output_stream = output_stream;
	}

	void change_output_stream(std::ostream& output_stream) noexcept
	{ change_output_stream(&output_stream); }

	[[nodiscard]] std::ostream& output_stream() const
	{ return *m_output_stream; }
};
}// namespace internal

using config = fgl::singleton<fgl::debug::output::internal::config>;

}// namespace fgl::debug

#endif // FGL_DEBUG_OUTPUT_HPP_INCLUDED
