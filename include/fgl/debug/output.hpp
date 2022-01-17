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
		fgl::debug::output::config::instance().change_output_stream(std::cerr);

		// example of something sending input to the stream
		fgl::debug::output::config::instance().output_stream() << "derp";

		// only output warnings and above
		fgl::debug::output::config::output_threshold =
			fgl::debug::output::warn;
	}
*/

#include <cassert>
#include <string_view>
#include <iostream>
#include <sstream>
#include <array>
#include <source_location>
#include <functional> // std::function
#include <cstdint> // uint_fast8_t

#include "../types/singleton.hpp"
#include "../types/ctypes.hpp" // cstring

namespace fgl::debug::output {

// user-defined channels beyond MAX are supported: "custom" channels
enum channel_e : uint_fast8_t
{
	echo = 0,
	fixme,
	debug,
	info,
	warn,
	error,
	MAX
};

/// CAUTION : this will become a constexpr string in future versions
static constexpr std::array<fgl::ctypes::cstring, channel_e::MAX>
channel_strings
{
	"ECHO",
	"FIXME",
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR"
};

namespace internal {
class config final
{
	FGL_SINGLETON_BOILERPLATE(config);
public:
	channel_e output_threshold{ channel_e::MAX };

	[[nodiscard]] bool channel_is_enabled(const channel_e channel) const noexcept
	{
		return output_threshold >= channel;
	}

	static std::string default_formatter(
		const channel_e channel,
		const std::string_view message,
		const std::source_location source)
	{ // TODO this can be constexpr when std::string::operator+ is implemented?
		std::stringstream ss;
		ss
			<< '['
			<< (channel < channel_e::MAX ? channel_strings[channel] : "CUSTOM")
			<< ']'
			<< " file:" << source.file_name()
			<< '(' << source.line() << ':' << source.column() << ") '"
			<< source.function_name()
			<< (message.empty() ? "" : "\n \\_____ ") << message;
		return ss.str();
	}

	void change_output_stream(std::ostream* const output_stream) noexcept
	{
		assert(output_stream != nullptr);
		m_output_stream = output_stream;
	}

	void change_output_stream(std::ostream& output_stream) noexcept
	{ change_output_stream(&output_stream); }

	[[nodiscard]] std::ostream& output_stream() const
	{ return *m_output_stream; }

private:
	std::ostream* m_output_stream{ &std::cout };
};
}// namespace internal

using config = fgl::singleton<fgl::debug::output::internal::config>;

using default_formatter_t =
	std::function<decltype(internal::config::default_formatter)>;

}// namespace fgl::debug::output

#endif // FGL_DEBUG_OUTPUT_HPP_INCLUDED
