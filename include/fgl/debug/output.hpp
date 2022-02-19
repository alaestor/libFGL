#pragma once
#ifndef FGL_DEBUG_OUTPUT_HPP_INCLUDED
#define FGL_DEBUG_OUTPUT_HPP_INCLUDED

/// QUICK-START GUIDE / EXAMPLE PROGRAM
/*
	#include <fgl/debug/output.hpp>

	int main()
	{
		// make a reference to the output configuration singleton
		auto& config{ fgl::debug::output::config::instance() };

		// redirect libFGL debug output to std::cerr
		config.change_output_stream(std::cerr);

		// only output warnings and above
		using priority = fgl::debug::output::priority_e;
		config.change_output_threshold(priority::warn);

		// send something to the output stream
		config.output_stream() << "Hello, world!" << std::endl;
	}
*/
/// EXAMPLE OUTPUT
/*
Hello, world!
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

// user-defined channels priorities MAX are supported: "custom" channels
enum priority_e : uint_fast8_t
{
	ALL = 0,
	echo,
	fixme,
	debug,
	info,
	warn,
	error,
	MAX
};

/// CAUTION : this will become a constexpr string in future versions
static constexpr std::array<fgl::cstring, priority_e::MAX> priority_strings
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

	priority_e m_priority_threshold{ priority_e::MAX };
	std::ostream* m_output_stream{ &std::cout };
public:

	[[nodiscard]] priority_e priority_threshold() const noexcept
	{ return m_priority_threshold; }

	void change_priority_threshold(const priority_e priority) noexcept
	{ m_priority_threshold = priority; }

	[[nodiscard]] bool above_priority_threshold(const priority_e priority)
	const noexcept
	{
		return m_priority_threshold >= priority;
	}

	static std::string default_formatter(
		const priority_e priority,
		const std::string_view message,
		const std::source_location source)
	{ // TODO this can be constexpr when std::string::operator+ is implemented?
		std::stringstream ss;
		ss
			<< '['
			<< (priority < priority_e::MAX
				? priority_strings[priority]
				: "CUSTOM")
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
};
}// namespace internal

using config = fgl::singleton<fgl::debug::output::internal::config>;

using default_formatter_t =
	std::function<decltype(internal::config::default_formatter)>;

}// namespace fgl::debug::output

#endif // FGL_DEBUG_OUTPUT_HPP_INCLUDED
