#pragma once
#ifndef FGL_DEBUG_STOPWATCH_HPP_INCLUDED
#define FGL_DEBUG_STOPWATCH_HPP_INCLUDED
#include "../environment/libfgl_compatibility_check.hpp"

/// QUICK-START GUIDE
/*
using namespace fgl::debug;
// reserve is used by an internal vector to avoid reallocations
stopwatch sw("name", reserve);
// if no name is provided, stopwatch will be named by the source location
// if no `reserve` is provided, defaults 1000 laps

sw.start();
do_a_thing();
sw.stop();
// uses output_config<stopwatch>::format_duration
std::cout << duration_to_string(sw.previous_lap()) << std::endl;
sw.reset();

sw.start()
for (;cond();)
{
	do_a_thing();
	sw.lap(); // each lap includes cond() time
}
// stop without recording so the last lap() is accurate
sw.stop_without_record();

// send statistics to debug output stream
fgl::debug::output(sw);
*/
/// EXAMPLE PROGRAM
/*
#include <iostream>
#include <thread>

#define NDEBUG // eliminate some stopwatch assertions
#include <fgl/debug/stopwatch.hpp>

int main()
{
	fgl::debug::stopwatch sw;
	std::this_thread::yield(); // warmup
	for (int i{}; i < 100; ++i)
	{
		sw.start();
		std::this_thread::yield();
		sw.stop();
	}

	std::cout << "cout: "<< sw << '\n' << std::endl;

	fgl::debug::output(sw); // outputs statistics by default

	// flush because the program terminates right after this
	fgl::debug::output::stream().flush();
}
*/
/// EXAMPLE PROGRAM OUTPUT
/*
cout: yield timer: 11µs 200ns  (100 laps)

[STOPWATCH]
 \_____ Statistics: yield timer
        Number of laps: 100
        Total elapsed: 11µs 200ns
        Mean lap:      112ns
        Median lap:    100ns
        Min lap:       100ns
        Max lap:       200ns
*/

#include <chrono>
#include <string>
#include <string_view>
#include <functional> // function
#include <sstream>
#include <ostream>
#include <iomanip> // setw
#include <algorithm> // sort
#include <numeric> // reduce
#include <vector>
#include <source_location>

#include <fgl/environment/build_type.hpp>
#include <fgl/debug/constexpr_assert.hpp>
#include <fgl/debug/output.hpp>

namespace fgl::debug {

template <typename T_clock = std::chrono::steady_clock>
requires std::chrono::is_clock_v<T_clock> && T_clock::is_steady
class generic_stopwatch
{
public:
	using clock_t = T_clock;
	using time_point_t = std::chrono::time_point<clock_t>;
	using duration_t = T_clock::duration;

private:
	// state is an artificial limit. helps by sanity-checking usage.
	enum class state : unsigned char
	{
		reset,
		ticking,
		stopped
	};

	// note: state shouldn't change or be checked if NDEBUG is defined
	state m_state;
	time_point_t m_last_point;
	std::vector<duration_t> m_record;

public:
	std::string name;

	[[nodiscard]] constexpr explicit
	generic_stopwatch(
		const std::string_view in_name,
		const std::size_t reserve = 1000)
	: m_state(state::reset), m_last_point{}, m_record{}, name(in_name)
	{ m_record.reserve(reserve); }

	// if a name isn't provided, one will be created from the source location.
	[[nodiscard]] constexpr explicit generic_stopwatch(
		const std::source_location sl = std::source_location::current(),
		const std::size_t reserve = 1000)
	:
		generic_stopwatch(
			std::string(sl.function_name()) + " in " + sl.file_name(),
			reserve
		)
	{}

	constexpr generic_stopwatch(const generic_stopwatch& sw) = default;
	constexpr generic_stopwatch(generic_stopwatch&& sw) = default;

	constexpr void start(const time_point_t time_point)
	{
		FGL_DEBUG_CONSTEXPR_ASSERT(m_state != state::ticking);
		if constexpr (fgl::debug_build)
			m_state = state::ticking;
		m_last_point = time_point;
	}

	void start()
	{ start(clock_t::now()); }

	constexpr void lap(const time_point_t time_point)
	{
		FGL_DEBUG_CONSTEXPR_ASSERT(m_state == state::ticking);
		FGL_DEBUG_CONSTEXPR_ASSERT(time_point >= m_last_point);
		m_record.emplace_back(time_point - m_last_point);
		m_last_point = time_point;
	}

	void lap()
	{ lap(clock_t::now()); }

	constexpr void stop(const time_point_t time_point)
	{
		FGL_DEBUG_CONSTEXPR_ASSERT(m_state == state::ticking);
		FGL_DEBUG_CONSTEXPR_ASSERT(time_point >= m_last_point);
		if constexpr (fgl::debug_build)
			m_state = state::stopped;
		m_record.emplace_back(time_point - m_last_point);
	}

	void stop()
	{ stop(clock_t::now()); }

	constexpr void stop_without_record() noexcept
	{
		FGL_DEBUG_CONSTEXPR_ASSERT(m_state == state::ticking);
		if constexpr (fgl::debug_build)
			m_state = state::stopped;
	}

	constexpr void reset()
	{
		m_record.clear();
		if constexpr (fgl::debug_build)
			m_state = state::reset;
	}

	[[nodiscard]] constexpr std::size_t number_of_laps() const
	{ return m_record.size(); }

	[[nodiscard]] constexpr
	duration_t get_lap(const std::size_t lap_number) const
	{
		FGL_DEBUG_CONSTEXPR_ASSERT(m_state == state::stopped);
		return m_record.at(lap_number);
	}

	[[nodiscard]] constexpr duration_t previous_lap() const
	{
		FGL_DEBUG_CONSTEXPR_ASSERT(m_state == state::stopped);
		return m_record.back();
	}

	[[nodiscard]] constexpr
	const std::vector<duration_t>& get_all_laps() const
	{
		FGL_DEBUG_CONSTEXPR_ASSERT(m_state == state::stopped);
		return m_record;
	}

	[[nodiscard]] constexpr duration_t elapsed() const
	{
		FGL_DEBUG_CONSTEXPR_ASSERT(m_state > state::reset);
		return std::reduce(
			m_record.cbegin(),
			m_record.cend(),
			duration_t{}
		);
	}

	[[nodiscard]] constexpr
	duration_t elapsed_to_lap(const std::size_t lap_number) const
	{
		FGL_DEBUG_CONSTEXPR_ASSERT(m_state > state::reset);
		return std::reduce(
			m_record.cbegin(),
			m_record.cbegin()
				+ static_cast<decltype(m_record)::difference_type>(lap_number),
			duration_t{}
		);
	}

	[[nodiscard]] constexpr
	duration_t elapsed_since_lap(const std::size_t lap_number) const
	{
		FGL_DEBUG_CONSTEXPR_ASSERT(m_state > state::reset);
		FGL_DEBUG_CONSTEXPR_ASSERT(lap_number < m_record.size());
		return std::reduce(
			m_record.cbegin()
				+ static_cast<decltype(m_record)::difference_type>(lap_number),
			m_record.cend(),
			duration_t{}
		);
	}

	struct statistics
	{
		std::size_t number_of_laps;
		duration_t total_elapsed;
		duration_t mean;
		duration_t median;
		duration_t min;
		duration_t max;

		[[nodiscard]] explicit constexpr
		statistics(const std::vector<duration_t>& sorted_laps) noexcept
		:
			number_of_laps{ sorted_laps.size() },
			total_elapsed{
				std::reduce(
					sorted_laps.cbegin(),
					sorted_laps.cend(),
					duration_t{}
				)
			},
			mean{ get_mean(sorted_laps) },
			median{ get_median(sorted_laps) },
			min{ sorted_laps.front() },
			max{ sorted_laps.back() }
		{
			FGL_DEBUG_CONSTEXPR_ASSERT(
				std::is_sorted(sorted_laps.cbegin(), sorted_laps.cend())
			);
		}

		private:

		[[nodiscard]] static constexpr
		duration_t get_mean(const std::vector<duration_t>& v) noexcept
		{
			const auto size{ static_cast<duration_t::rep>(v.size()) };

			return (size != 0)
				? std::reduce(v.cbegin(), v.cend(), duration_t{}) / size
				: duration_t{};
		}

		[[nodiscard]] static constexpr
		duration_t get_median(const std::vector<duration_t>& v) noexcept
		{
			using rep_t = typename duration_t::rep;
			const std::size_t size{ v.size() };
			switch (size)
			{
			break; case 0: return {};
			break; case 1: return { v[0] };
			break; case 2: return { (v[0] + v[1]) / rep_t{2} };
			break; default:
				const std::size_t mid{ size / rep_t{2} };
				return {
					(size % 2 == 0)
					? (v[mid-1] + v[mid]) / rep_t{2}
					: v[mid]
				};
			}
		}
	};

	[[nodiscard]] constexpr statistics calculate_statistics() const
	{
		FGL_DEBUG_CONSTEXPR_ASSERT(m_state == state::stopped);
		std::vector<duration_t> v(m_record);
		std::sort(v.begin(), v.end());
		return statistics(v);
	}
};

using stopwatch = generic_stopwatch<std::chrono::steady_clock>;

template <typename T_clock>
class output_config<generic_stopwatch<T_clock>>
{
	static inline bool m_enabled{ true };
	public:
	using stopwatch_t = generic_stopwatch<T_clock>;
	output_config(auto&&...) = delete;
	~output_config() = delete;
	static void turn_on() { m_enabled = true; }
	static void turn_off() { m_enabled = false; }
	static bool enabled() { return m_enabled; }
	static priority get_priority() { return priority::info; }
	static std::string_view name() { return "STOPWATCH"; }

	[[nodiscard]] static std::string format(const stopwatch_t& sw)
	{
		std::stringstream ss;
		ss
			<< "Statistics: " << sw.name << '\n'
			<< statistics_formatter(sw.calculate_statistics());
		return output::default_fmt_msg(ss.str());
	}

	[[nodiscard]] static
	std::string default_duration_formatter(stopwatch_t::duration_t duration)
	{
		std::stringstream ss;
		const auto process{
			[&duration, &ss](const auto unit_time) -> void
			{
				if (unit_time.count() != 0)
				{
					duration -= unit_time;
					ss << unit_time << ' ';
				}
			}
		};
		using namespace std::chrono;
		process(duration_cast<years>(duration));
		process(duration_cast<months>(duration));
		process(duration_cast<weeks>(duration));
		process(duration_cast<days>(duration));
		process(duration_cast<hours>(duration));
		process(duration_cast<minutes>(duration));
		process(duration_cast<seconds>(duration));
		process(duration_cast<milliseconds>(duration));
		process(duration_cast<microseconds>(duration));
		process(duration_cast<nanoseconds>(duration));
		assert(duration.count() == 0);
		return ss.str();
	}

	[[nodiscard]] static std::string default_statistics_formatter(
		const stopwatch_t::statistics& stats)
	{
		std::stringstream ss;
		ss
			<< "\tNumber of laps: " << stats.number_of_laps
			<< "\n\tTotal elapsed: " << duration_formatter(stats.total_elapsed)
			<< "\n\tMean lap:      " << duration_formatter(stats.mean)
			<< "\n\tMedian lap:    " << duration_formatter(stats.median)
			<< "\n\tMin lap:       " << duration_formatter(stats.min)
			<< "\n\tMax lap:       " << duration_formatter(stats.max);
		return ss.str();
	}

	[[nodiscard]] static
	std::string default_stopwatch_formatter(const stopwatch_t& sw)
	{
		std::stringstream ss;
		const std::size_t n{ sw.number_of_laps() };
		ss
			<< sw.name << ": " << duration_formatter(sw.elapsed())
			<< " (" << n << " lap" << (n == 1 ? "" : "s") << ")";
		return ss.str();
	}

	using duration_formatter_t =
		std::function<decltype(default_duration_formatter)>;

	using statistics_formatter_t =
		std::function<decltype(default_statistics_formatter)>;

	using stopwatch_formatter_t =
		std::function<decltype(default_stopwatch_formatter)>;

	static inline duration_formatter_t duration_formatter{
		default_duration_formatter
	};

	static inline statistics_formatter_t statistics_formatter{
		default_statistics_formatter
	};

	static inline stopwatch_formatter_t stopwatch_formatter{
		default_stopwatch_formatter
	};
};

static_assert(output_handler<output_config<stopwatch>, stopwatch>);

template <typename T_clock>
std::ostream& operator<<(std::ostream& os, const generic_stopwatch<T_clock>& sw)
{ return os << output_config<generic_stopwatch<T_clock>>::stopwatch_formatter(sw); }

/// Helper functions to get formatted strings

// TODO test this; do i need an is_duration?
template <typename T_clock>
[[nodiscard]] inline std::string to_string_duration(
	const typename generic_stopwatch<T_clock>::duration_t duration)
{
	return output_config<generic_stopwatch<T_clock>>::duration_formatter(
		duration
	);
}

template <typename T_clock>
[[nodiscard]] inline std::string to_string_minimal(
	const generic_stopwatch<T_clock>& sw)
{
	return output_config<generic_stopwatch<T_clock>>::stopwatch_formatter(sw);
}

template <typename T_clock>
[[nodiscard]] inline std::string to_string_statistics(
	const generic_stopwatch<T_clock>& sw)
{
	return
		sw.name
		+ "\n \\_____ Statistics\n"
		+ output_config<generic_stopwatch<T_clock>>::statistics_formatter(
			sw.calculate_statistics()
		);
}

template <typename T_clock>
[[nodiscard]] inline std::string to_string_full(
	const generic_stopwatch<T_clock>& sw)
{
	std::stringstream ss;
	ss << to_string_statistics(sw);
	if (const std::size_t nlaps{ sw.number_of_laps() }; nlaps > 0)
	{
		// number of digits used to represent the highest lap number
		const int max_digits{
			[](std::size_t value) -> int
			{ // not size_t because std::setw(int)
				int number_columns{ 1 };
				for (; value > 9; value /= 10, ++number_columns);
				return number_columns;
			}(nlaps - 1) // laps count from 0
		};

		ss << "     \\_ Lap durations\n";
		for (std::size_t i{}; const auto lap : sw.get_all_laps())
		{
			ss
				<< "\tLap " << std::setw(max_digits) << i++ << ": "
				<< output_config<generic_stopwatch<T_clock>>::duration_formatter(
					lap)
				<< '\n';
		}
	}
	return ss.str();
}

} // namespace fgl::debug

#endif // FGL_DEBUG_STOPWATCH_HPP_INCLUDED
