#pragma once
#ifndef FGL_TYPES_UNITS_DATA_LENGTHS_HPP_INCLUDED
#define FGL_TYPES_UNITS_DATA_LENGTHS_HPP_INCLUDED

#include <cstdint>
#include <cstddef> // size_t
#include <ratio>
#include <concepts> // same_as
#include <limits>

#include "../math/pow.hpp"

namespace fgl::traits {

template <typename T>
struct is_ratio : public std::false_type
{};

template <std::intmax_t T_nom, std::intmax_t T_den>
struct is_ratio<std::ratio<T_nom, T_den>> : public std::true_type
{};

template <typename T>
inline constexpr bool is_ratio_v = is_ratio<T>::value;

template <typename T>
concept ratio_type = is_ratio_v<T>;

} // namespace fgl::traits

namespace fgl::units {

/// Generic unit type for data sizes

template <fgl::traits::numeric_type T_rep, fgl::traits::ratio_type T_ratio>
struct data_size
{
	using rep = T_rep;
	using byte_ratio = typename T_ratio::type;

	[[nodiscard]] static constexpr rep zero() noexcept { return rep{0}; }

	[[nodiscard]] static constexpr rep min() noexcept
	{ return rep{std::numeric_limits<rep>::min()}; }

	[[nodiscard]] static constexpr rep max() noexcept
	{ return rep{std::numeric_limits<rep>::max()}; }

	rep value;
	[[nodiscard]] constexpr rep count() const { return value; }
};

template <typename T>
struct is_data_size : public std::false_type
{};

template <typename T_rep, typename T_ratio>
struct is_data_size<data_size<T_rep, T_ratio>> : public std::true_type
{};

template <typename T>
inline constexpr bool is_data_size_v = is_data_size<T>::value;

template <typename T>
concept data_size_type = is_data_size_v<T>;

template<data_size_type T_out, data_size_type T_in>
constexpr T_out data_size_cast(const T_in& in_data_size)
{
	using ratio_divide = std::ratio_divide<
		typename T_in::byte_ratio,
		typename T_out::byte_ratio
	>;

	using common_type = std::common_type_t<
		typename T_out::rep,
		typename T_in::rep,
		intmax_t
	>;

	constexpr bool num_is_one{ ratio_divide::num == 1 };
	constexpr bool den_is_one{ ratio_divide::den == 1 };

	if constexpr (num_is_one && den_is_one)
	{
		return T_out{ static_cast<T_out::rep>(in_data_size.count()) };
	}
	else if constexpr (num_is_one && !den_is_one)
	{
		return T_out{
			static_cast<T_out::rep>(
				static_cast<common_type>(in_data_size.count())
				/ static_cast<common_type>(ratio_divide::den)
			)
		};
	}
	else if constexpr (!num_is_one && den_is_one)
	{
		return T_out{
			static_cast<T_out::rep>(
				static_cast<common_type>(in_data_size.count())
				* static_cast<common_type>(ratio_divide::num)
			)
		};
	}
	else // (!num_is_one && !den_is_one)
	{
		return T_out{
			static_cast<T_out::rep>(
				static_cast<common_type>(in_data_size.count())
				* static_cast<common_type>(ratio_divide::num)
				/ static_cast<common_type>(ratio_divide::den)
			)
		};
	}
}


/// Units


namespace ratio
{

using bit = std::ratio<1,8>;
using nibble = std::ratio<1,4>;
using byte = std::ratio<1,1>;

// Metric ratios borrowed from STL <ratio>

using std::kilo, std::mega, std::giga, std::tera, std::peta, std::exa;
using kilobit = std::ratio_multiply<kilo, bit>;
using megabit = std::ratio_multiply<mega, bit>;
using gigabit = std::ratio_multiply<giga, bit>;
using terabit = std::ratio_multiply<tera, bit>;
using petabit = std::ratio_multiply<peta, bit>;
using exabit = std::ratio_multiply<exa, bit>;
using kilobyte = std::ratio_multiply<kilo, byte>;
using megabyte = std::ratio_multiply<mega, byte>;
using gigabyte = std::ratio_multiply<giga, byte>;
using terabyte = std::ratio_multiply<tera, byte>;
using petabyte = std::ratio_multiply<peta, byte>;
using exabyte = std::ratio_multiply<exa, byte>;

// IEC ratios

using kibi = std::ratio<fgl::pow<intmax_t>(1024, 1), 1>;
using mibi = std::ratio<fgl::pow<intmax_t>(1024, 2), 1>;
using gibi = std::ratio<fgl::pow<intmax_t>(1024, 3), 1>;
using tibi = std::ratio<fgl::pow<intmax_t>(1024, 4), 1>;
using pibi = std::ratio<fgl::pow<intmax_t>(1024, 5), 1>;
using eibi = std::ratio<fgl::pow<intmax_t>(1024, 6), 1>;
using kibibit = std::ratio_multiply<kibi, bit>;
using mebibit = std::ratio_multiply<mibi, bit>;
using gibibit = std::ratio_multiply<gibi, bit>;
using tebibit = std::ratio_multiply<tibi, bit>;
using pebibit = std::ratio_multiply<pibi, bit>;
using exbibit = std::ratio_multiply<eibi, bit>;
using kibibyte = std::ratio_multiply<kibi, byte>;
using mebibyte = std::ratio_multiply<mibi, byte>;
using gibibyte = std::ratio_multiply<gibi, byte>;
using tebibyte = std::ratio_multiply<tibi, byte>;
using pebibyte = std::ratio_multiply<pibi, byte>;
using exbibyte = std::ratio_multiply<eibi, byte>;

} // namespace ratios

// common units

using bits = data_size<intmax_t, ratio::bit>;
using bytes = data_size<intmax_t, ratio::byte>;

// metric units

using kilobits = data_size<intmax_t, ratio::kilobit>;
using megabits = data_size<intmax_t, ratio::megabit>;
using gigabits = data_size<intmax_t, ratio::gigabit>;
using terabits = data_size<intmax_t, ratio::terabit>;
using petabits = data_size<intmax_t, ratio::petabit>;
using exabits = data_size<intmax_t, ratio::exabit>;
using kilobytes = data_size<intmax_t, ratio::kilobyte>;
using megabytes = data_size<intmax_t, ratio::megabyte>;
using gigabytes = data_size<intmax_t, ratio::gigabyte>;
using terabytes = data_size<intmax_t, ratio::terabyte>;
using petabytes = data_size<intmax_t, ratio::petabyte>;
using exabytes = data_size<intmax_t, ratio::exabyte>;

// IEC units

using kibibits = data_size<intmax_t, ratio::kibibit>;
using mebibits = data_size<intmax_t, ratio::mebibit>;
using gibibits = data_size<intmax_t, ratio::gibibit>;
using tebibits = data_size<intmax_t, ratio::tebibit>;
using pebibits = data_size<intmax_t, ratio::pebibit>;
using exbibits = data_size<intmax_t, ratio::exbibit>;
using kibibytes = data_size<intmax_t, ratio::kibibyte>;
using mebibytes = data_size<intmax_t, ratio::mebibyte>;
using gibibytes = data_size<intmax_t, ratio::gibibyte>;
using tebibytes = data_size<intmax_t, ratio::tebibyte>;
using pebibytes = data_size<intmax_t, ratio::pebibyte>;
using exbibytes = data_size<intmax_t, ratio::exbibyte>;


/// Formatting


namespace internal {
template <typename T_ratio>
consteval auto data_size_unit_suffix()
{
	using namespace fgl::units::ratio;
	if constexpr (std::same_as<T_ratio, ratio::bit>) return "b";
	if constexpr (std::same_as<T_ratio, ratio::byte>) return "B";
	else if constexpr (T_ratio::num % 1024)
	{ // IEC
		if constexpr (std::same_as<T_ratio, kibibit>) return "Kib";
		else if constexpr (std::same_as<T_ratio, mebibit>) return "Mib";
		else if constexpr (std::same_as<T_ratio, gibibit>) return "Gib";
		else if constexpr (std::same_as<T_ratio, tebibit>) return "Tib";
		else if constexpr (std::same_as<T_ratio, pebibit>) return "Pib";
		else if constexpr (std::same_as<T_ratio, exbibit>) return "Eib";
		else if constexpr (std::same_as<T_ratio, kibibyte>) return "KiB";
		else if constexpr (std::same_as<T_ratio, mebibyte>) return "MiB";
		else if constexpr (std::same_as<T_ratio, gibibyte>) return "GiB";
		else if constexpr (std::same_as<T_ratio, tebibyte>) return "TiB";
		else if constexpr (std::same_as<T_ratio, pebibyte>) return "PiB";
		else if constexpr (std::same_as<T_ratio, exbibyte>) return "EiB";
	}
	else // Metric
	{
		if constexpr (std::same_as<T_ratio, kilobit>) return "Kb";
		else if constexpr (std::same_as<T_ratio, megabit>) return "Mb";
		else if constexpr (std::same_as<T_ratio, gigabit>) return "Gb";
		else if constexpr (std::same_as<T_ratio, terabit>) return "Tb";
		else if constexpr (std::same_as<T_ratio, petabit>) return "Pb";
		else if constexpr (std::same_as<T_ratio, exabit>) return "Eb";
		else if constexpr (std::same_as<T_ratio, kilobyte>) return "KB";
		else if constexpr (std::same_as<T_ratio, megabyte>) return "MB";
		else if constexpr (std::same_as<T_ratio, gigabyte>) return "GB";
		else if constexpr (std::same_as<T_ratio, terabyte>) return "TB";
		else if constexpr (std::same_as<T_ratio, petabyte>) return "PB";
		else if constexpr (std::same_as<T_ratio, exabyte>) return "EB";
	}
	return "";
}

} // namespace internal

} // namespace fgl::units

#endif // FGL_TYPES_UNITS_DATA_LENGTHS_HPP_INCLUDED
