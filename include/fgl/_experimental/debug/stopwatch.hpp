#pragma once
#ifndef FGL_DEBUG_STOPWATCH_HPP_INCLUDED
#define FGL_DEBUG_STOPWATCH_HPP_INCLUDED

/// This will probably change a lot in the future.
// Included for sake of completeness

#include <chrono>
#include <vector>
#include <stdexcept> // runtime_error, invalid_argument
#include <string_view>
#include <ostream>
#include <sstream>

namespace fgl::debug {

template <typename ODR = void>
class Stopwatch
{
	typedef std::chrono::steady_clock clock;

	const std::string m_name;
	const std::chrono::time_point<clock> m_unset{};
	std::vector<std::chrono::time_point<clock>> m_laps;
	std::chrono::time_point<clock> m_start, m_stop;

	[[nodiscard]] static std::string format_chrono_duration(
		const std::chrono::nanoseconds elapsed)
	{
		using // using namespace std::chrono doesn't include nanoseconds?
			std::chrono::duration_cast,
			std::chrono::nanoseconds,
			std::chrono::microseconds,
			std::chrono::milliseconds,
			std::chrono::seconds,
			std::chrono::minutes,
			std::chrono::hours;

		std::ostringstream os;
		auto remaining{ elapsed };

		const auto process{
			[&remaining, &os]<typename T>(T time, std::string_view abbrev)
			{
				remaining -= time;
				if (const auto& t{ time.count() }; t > 0)
					os << t << " ";
			}
		};

		process(duration_cast<hours>(remaining), "h");
		process(duration_cast<minutes>(remaining), "m");
		process(duration_cast<seconds>(remaining), "s");
		process(duration_cast<milliseconds>(remaining), "ms");
		process(duration_cast<microseconds>(remaining), "us");
		process(duration_cast<nanoseconds>(remaining), "ns");

		if (remaining.count() > 0)
			throw std::runtime_error("fgl::debug duration format failure");

		return os.str();
	}

public:
	void start()
	{ m_start = clock::now(); }

	void stop()
	{ m_stop = clock::now(); }

	void reset()
	{ m_start = m_stop = m_unset; }

	[[nodiscard]]
	std::string_view getName() const
	{ return m_name; }

	void lap()
	{
		if (m_start == m_unset)
			throw std::runtime_error(m_name+" must start before you can lap!");

		m_laps.push_back(clock::now());
	}

	void clearLaps()
	{ m_laps.clear(); }

	[[nodiscard]] std::size_t numberOfLaps() const noexcept
	{ return m_laps.size(); }

	[[nodiscard]] std::string getLap(const std::size_t number) const
	{
		if (m_laps.size() == 0)
			throw std::runtime_error(m_name+" has no laps");

		if (number == 0)
			throw std::invalid_argument(m_name+" getLap number must be > 0");

		const std::size_t i{ number-1 };

		if (i > m_laps.size())
			throw std::invalid_argument(m_name+" getLap number out of range");

		const auto lap{ m_laps.at(i) };
		const auto lastLap{ i > 1 ? m_laps.at(i-1) : m_start };
		std::stringstream sstream;
		sstream
			<< m_name << " lap " << number << ": "
			<< format_chrono_duration(lap - lastLap);

		return sstream.str();
	}

	[[nodiscard]] std::string previousLap() const
	{ return getLap(m_laps.size()); }

	[[nodiscard]] std::string allLaps() const
	{
		std::stringstream sstream;

		auto lastLap{ m_start };
		std::size_t counter{ 1 };
		for (const auto& lap : m_laps)
		{
			sstream
				<< m_name << " Lap " << counter << ": "
				<< formatted(lap - lastLap) << "\n";
			lastLap = lap;
			++counter;
		}

		return sstream.str();
	}

	[[nodiscard]] std::string averageLaps() const
	{
		if (m_laps.size() == 0)
			throw std::runtime_error(m_name+" has no laps");

		std::chrono::nanoseconds accum{ std::chrono::nanoseconds::zero() };
		auto lastLap{ m_start };
		for (const auto& lap : m_laps)
		{
			accum += lap - lastLap;
			lastLap = lap;
		}

		const auto avg{ accum / m_laps.size() };

		return formatted(avg);
	}

	friend std::ostream& operator<<(std::ostream& os, const Stopwatch& sw)
	{
		return os
			<< sw.m_name << ": "
			<< sw.formatted(sw.m_stop - sw.m_start);
	}

	[[nodiscard]]
	explicit Stopwatch(std::string_view name, const std::size_t reserve = 1000)
	:
		m_name(name)
	{
		m_laps.reserve(reserve);
		static_assert(clock::is_steady,
			"stopwatch chrono::steady_clock is not steady; not OS supported?");
	}

	~Stopwatch() = default;
};
}// namespace fgl::debug

#endif // FGL_DEBUG_STOPWATCH_HPP_INCLUDED
