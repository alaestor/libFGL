#pragma once
#ifndef FGL_DEBUG_OUTPUT_HPP_INCLUDED
#define FGL_DEBUG_OUTPUT_HPP_INCLUDED
#include "../environment/libfgl_compatibility_check.hpp"

/// QUICK-START GUIDE
/*
	While intended as a "debug output stream", the libFGL output system could
	be used for any purpose, including logging. For this reason, several output
	priorities and thresholds are provided (`fgl::debug::priority` enum).

	Use `fgl::debug::output()` to send output to the libFGL output stream.
	Generic channels will be instantiated for every type output, and are
	configurable via `fgl::debug::output_config<T>`.

	The generic `output_config<T>` default formatter is defined to use the
	`operator<<(std::ostream&, T)` but can be customized by assigning to the
	`std::function` member `output_config<T>::format`.

	Generic output can be disabled from appearing entirely by assigning to the
	`fgl::debug::generic_output_disabled` boolean.

	For more control over output and type-specific formatting, the user can
	define static `output_channel`s and `output_formatter`s for specific types.

	This allows for use of `output::custom<T, T_channel, T_formatter>()`.

	A static `output_channel` must have an enable state, name, and priority.
	A static `output_formatter` must have a format function for `T`.
	A static `output_handler` is the combination of a formatter and a channel.

	An `output_handler` is required to use the `output::handled<T_handler>()`
	method, which is equivalent to `output::custom<T, T_handler, T_handler>()`.

	The convention is to specialize `output_config<T>` as an `output_handler`.
	If that's case, you can use the `output::operator()` which is equivalent to
	`output::handled<T, output_config<T>>(t)`.

	Whether or not a channel can output (`output::can_send<T_channel>()`)
	is determined by three factors: the channel's priority, the channel's
	`enabled()` state, and the `output::enabled` boolean.

	`output::priority_threshold` gets/sets the minimum priority for channels to
	output. Only channels with a priority greater than or equal to this value
	will recieve the output stream via `output::channel_stream<T_channel>`.

	use `output::stream()` to get/set the libFGL output stream.

	`output` contains several configurable formatters (`format_head`,
	`format_msg`, `format_msg_src`) and also defines their defaults.

	Most libFGL components that interact with libFGL output will have
	customization settings available in a `output_config<T>` specialization.
*/
/// EXAMPLE PROGRAM
/*
#include <string>
#include <sstream>

#include <fgl/debug/output.hpp>

// This example shows a more complex usecase of a template. For concete types,
// just specialize `output_config` to satisfy the `output_handler` concept

// the user defined type; a template in this case.
template <typename T_position = double>
struct entity
{
	std::string name;
	T_position position[3];
};

// an output channel for the user defined type
class entity_channel
{
	static inline bool m_enabled{ true };
	public:
	// should never be instantiated
	entity_channel(auto&&...) = delete;
	~entity_channel() = delete;

	// mechanisms to satisfy the output_channel concept
	static void turn_on() { m_enabled = true; }
	static void turn_off() { m_enabled = false; }
	static bool enabled() { return m_enabled; }
	static std::string_view name() { return "Entity"; }

	static fgl::debug::priority get_priority()
	{ return fgl::debug::priority::debug; }
};

static_assert(fgl::debug::output_channel<entity_channel>);

// an output formatter for the user defined type (a template in this case)
template <typename T_position>
struct entity_formatter
{
	entity_formatter(auto&&...) = delete;
	~entity_formatter() = delete;
	static std::string format(const entity<T_position>& entity)
	{
		std::ostringstream ss;
		ss
			<< entity.name << " at coordinates ("
			<< entity.position[0] << ","
			<< entity.position[1] << ","
			<< entity.position[2] << ")";
		return ss.str();
	}
};

// make sure we satisfy the formatter concept
static_assert(
	fgl::debug::output_formatter<entity_formatter<double>, entity<double>>
);


// now that we have a channel and formatter, we could use them directly via
// `output::custom<T, T_channel, T_formatter>(t)`, but that's quite cumbersome.

// For concete types, it's libFGL convention to define the channel and
// formatter together in a specialization of `output_config` which satifies the
// `output_handler` concept; combining the channel and formatter.

// That allows us to the the `output::operator()` instead which is much easier,
// and means having a unified configuration interface via `output_config<T>`.

// We can abstract `entity_channel` and `entity_formatter` into a
// handler object which will redirect the apporpriate calls. This way all the
// `output_config<entity<T>>` synthesized objects use the same channel,
// but have different (template-synthesized) formatters.

// Now we can use `output::operator()` with any entity type, and all of the
// configuration can be done thru the handler.


template <typename T_position>
class fgl::debug::output_config<entity<T_position>>
{
	public:
	// redirect so all synethized output_config<T> types use the same channel.
	static void turn_on() { entity_channel::turn_on(); }
	static void turn_off() { entity_channel::turn_off(); }
	static bool enabled() { return entity_channel::enabled(); }
	static std::string_view name() { return entity_channel::name(); }

	static fgl::debug::priority get_priority()
	{ return entity_channel::get_priority(); }

	// call the appropriate formatter
	static std::string format(const entity<T_position>& entity)
	{ return entity_formatter<T_position>::format(entity); }
};

static_assert(
	fgl::debug::output_handler<
		fgl::debug::output_config<entity<double>>,
		entity<double>
	>
);

int main()
{
	entity player{ "Lexy", {0.0, 0.0, 0.0} };

	// set the output stream (redundant; defaults to std::cout)
	fgl::debug::output::stream(std::cout);

	// channel priority must be >= output threshold to display output
	fgl::debug::output::priority_threshold( // defaults to priority::minimum
		entity_channel::get_priority() // defined as priority::debug
	);

	// channel enabled() must return true to display output
	entity_channel::turn_on(); // redundant in this case, defaults to enabled

	// output player entity; appends a '\n' to the formatted output
	fgl::debug::output::custom<
		entity<double>,
		entity_channel,
		entity_formatter<double>
	>(player);

	// alternatively, since we satified the `output_handler` concept
	using edouble_handler_t = fgl::debug::output_config<entity<double>>;
	fgl::debug::output::handled<entity<double>, edouble_handler_t>(player);

	// but, even better since we specialized `output_config` as our handler:
	fgl::debug::output(player);

	// make sure we flush the stream since the program terminates after this
	fgl::debug::output::stream().flush();
}
*/
/// EXAMPLE OUTPUT
/*
[Entity] Lexy at coordinates (0,0,0)
[Entity] Lexy at coordinates (0,0,0)
[Entity] Lexy at coordinates (0,0,0)
*/


#include <cassert>
#include <string>
#include <string_view>
#include <iostream> // cout
#include <sstream> // ostringstream
#include <functional> // function, reference wrapper
#include <optional>
#include <source_location>


namespace fgl::debug {

enum class priority : unsigned char
{
	minimum,
	debug,
	info,
	message,
	event,
	warning,
	error,
	fatal,
	maximum
};

template <class T>
concept output_channel = requires
{
	{ T::get_priority() } -> std::same_as<priority>;
	{ T::name() } -> std::same_as<std::string_view>;
	{ T::enabled() } -> std::same_as<bool>;
	{ T::turn_on() } -> std::same_as<void>;
	{ T::turn_off() } -> std::same_as<void>;
};

template <class T_formatter, typename T_value>
concept output_formatter = requires (const T_value& t)
{
	{ T_formatter::format(t) } -> std::same_as<std::string>;
};

template <class T, typename T_value>
concept output_handler = output_channel<T> && output_formatter<T, T_value>;


static inline bool generic_output_disabled{ false };

template <typename T>
class output_config
{
	// generic acts as both a general channel and a formatter
	// switch generic_output_enabled to false to disable all generic channels
	static inline bool m_enabled{ true };
	public:
	output_config(auto&&...) = delete;
	~output_config() = delete;
	static void turn_on() { m_enabled = true; }
	static void turn_off() { m_enabled = false; }
	static bool enabled() { return m_enabled && not generic_output_disabled; }
	static priority get_priority() { return priority::info; }
	static std::string_view name() { return "GENERIC"; }

	static std::string default_formatter(const T& t)
	{
		std::ostringstream ss;
		ss << t;
		return ss.str();
	}

	static inline std::function<decltype(default_formatter)> format{
		default_formatter
	};
};

static_assert(output_handler<output_config<int>, int>);

// everything above here could become a separate file... but should it?

struct output final
{
	// toggle for all output
	static inline bool enabled{ true };

	// The priority of a channel must be >= this threshold to be sent.
	class priority_threshold_t
	{
		static inline priority m_threshold{ priority::minimum };
		public:

		void operator()(priority priority_threshold) noexcept
		{ m_threshold = priority_threshold; }

		[[nodiscard]] priority operator()() const noexcept
		{ return m_threshold; }

		auto operator<=>(const priority priority) const noexcept
		{ return m_threshold <=> priority; }
	}
	static inline priority_threshold;

	// the libFGL debug output stream
	class output_stream_t
	{
		static inline std::ostream* m_output_stream{ &std::cout };
		public:

		void operator()(std::ostream& output_stream) noexcept
		{ m_output_stream = &output_stream; }

		[[nodiscard]] std::ostream& operator()() const noexcept
		{ return *m_output_stream; }
	}
	static inline stream;

	// returns `true` if the output channel is permitted to send output
	template <output_channel T_channel>
	static bool can_send() noexcept
	{
		return
			enabled
			&& T_channel::enabled()
			&& priority_threshold <= T_channel::get_priority();
	}

	// Requests stream access for a given channel. Granted if `can_send()`
	template <output_channel T_channel>
	[[nodiscard]] static
	std::optional<std::reference_wrapper<std::ostream>> channel_stream()
	{
		return (can_send<T_channel>())
			? std::make_optional(std::ref(stream()))
			: std::nullopt;
	}

	// a default formatter for the channel name prefix
	[[nodiscard]] static inline std::string default_fmt_head(
		const std::string_view name)
	{
		std::ostringstream ss;
		ss << '[' << name << "] ";
		return ss.str();
	}

	// configurable formatter for the channel name prefix
	using format_head_t = std::function<decltype(default_fmt_head)>;
	static inline format_head_t format_head{ default_fmt_head };

	// default formatter for the channel name and message string
	[[nodiscard]] static inline std::string default_fmt_msg(
		const std::string_view message)
	{
		assert(not message.empty());
		std::ostringstream ss;
		ss
			<< "\n \\_____ " << message;
		return ss.str();
	}

	// configurable formatter for the channel name and message string
	using format_msg_t = std::function<decltype(default_fmt_msg)>;
	static inline format_msg_t format_msg{ default_fmt_msg };

	// default formatter for the channel name, message string, and source
	[[nodiscard]] static inline std::string default_fmt_msg_src(
		const std::string_view message,
		const std::source_location source)
	{
		std::ostringstream ss;
		ss
			<< "file:" << source.file_name()
			<< '(' << source.line() << ':' << source.column() << ") '"
			<< source.function_name()
			<< (message.empty() ? "" : "\n \\_____ ") << message;
		return ss.str();
	}

	// configurable formatter for the channel name, message string, and source
	using format_msg_src_t = std::function<decltype(default_fmt_msg_src)>;
	static inline format_msg_src_t format_msg_src{ default_fmt_msg_src };

	// attempt formatted output on T_channel. success if `can_send()`
	template
	<
		typename T,
		output_channel T_channel,
		output_formatter<T> T_formatter
	>
	static void custom(const T& t)
	{
		if (const auto& os{ channel_stream<T_channel>() }; os)
		{
			os.value().get()
				<< format_head(T_channel::name())
				<< T_formatter::format(t)
				<< '\n';
		}
	}

	// attempt output, providing channel and formatter via an output_handler
	template <typename T, output_handler<T> T_handler>
	static void handled(const T& t)
	{ custom<T, T_handler, T_handler>(t); }

	// attempt output, T must correspond to an output_config<T> output_handler
	template <typename T>
	requires output_handler<output_config<T>, T>
	void operator()(const T& t) const
	{ handled<T, output_config<T>>(t); }
};

[[maybe_unused]] static inline output output;

} // namespace fgl::debug

#endif // FGL_DEBUG_OUTPUT_HPP_INCLUDED
