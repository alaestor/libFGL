#pragma once
#ifndef FGL_UTILITY_BINARY_FILES_HPP_INCLUDED
#define FGL_UTILITY_BINARY_FILES_HPP_INCLUDED

/// QUICK-START GUIDE
/* Argument syntax: func(required, [optional])

	// reading, easy mode
	std::vector<std::byte> file_contents{ fgl::read_binary_file(file_path) };

	// reading, using an existing buffer & file_size as bytes_to_read
	// passing file_size is optional, but buffer length must be >= filesize
	const std::size_t file_size{ fgl::get_file_size(file_path) };
	std::vector<std::byte> file_contents(file_size, std::byte{});
	fgl::read_binary_file(file_path, file_contents, [file_size]);

	// writing
	write_binary_file(
		file_path, file_contents, [file_contents.size()], [std::ios::trunc]
	);

	Append `_noexcept` suffix to function names to use non-throwing variants.
	Note that return types and how they signal failure changes (read comments).

	All I/O ranges must be of a type that satisfies fgl::traits::byte_type
	(char, unsigned char, or std::byte)

	By default, `read_binary_file` without specifying an output range will
	return a `std::vector<T>` where `T` is `std::byte` by default.

	By default, `write_binary_file` will truncate (overwrite) existing files.
*/
/// EXAMPLE PROGRAM
/*
	#include <iostream>
	#include <cstddef> // byte, to_integer
	#include <array>
	#include <filesystem>

	#include <fgl/utility/files.hpp>

	int main()
	{
		constexpr std::array binary_data{ std::byte{'h'}, std::byte{'i'} };
		const std::filesystem::path file_path{ "file.bin" };

		fgl::write_binary_file(file_path, binary_data);

		const auto contents_in{ fgl::read_binary_file<char>(file_path) };

		for (const char c : contents_in)
			std::cout << c;

		std::cout << std::endl;
	}
*/
/// EXAMPLE OUTPUT
/*
hi
*/

#include <ranges> // contiguous_range
#include <string> // string, to_string
#include <stdexcept> // runtime_error
#include <concepts> // integral
#include <filesystem>
#include <fstream>
#include <optional>

#include "../types/traits.hpp" // byte_type

namespace fgl {

// Throws on failure. Simple casting wrapper for std::filesystem::file_size()
template <std::integral T = std::size_t> [[nodiscard]]
T get_file_size(const std::filesystem::path& file_path)
{
	return static_cast<T>(std::filesystem::file_size(file_path));
}

/*Returns std::numeric_limits<T>::max() on failure.
Simple casting wrapper for std::filesystem::file_size()*/
template <std::integral T = std::size_t> [[nodiscard]]
T get_file_size_noexcept(const std::filesystem::path& file_path)
noexcept
{
	std::error_code error{};
	const auto file_size{ std::filesystem::file_size(file_path, error) };
	return error ? std::numeric_limits<T>::max() : static_cast<T>(file_size);
}

/*Throws on failure.
If bytes_to_read is 0 it will be the size of the file obtained from filesystem.
The size of the output range must be >= bytes_to_read.*/
template <std::ranges::contiguous_range T_range>
requires fgl::traits::byte_type<std::ranges::range_value_t<T_range>>
void read_binary_file(
	const std::filesystem::path& file_path,
	T_range& output,
	const std::size_t bytes_to_read = 0)
	// wish i could `bytes_to_read = get_file_size(file_path)`, but no...
{
	const std::size_t buffer_size{ std::ranges::size(output) };
	const std::size_t read_size{
		(bytes_to_read > 0) ? bytes_to_read : get_file_size(file_path)
	};
	if (buffer_size >= read_size)
	{
		if (std::ifstream ifs(file_path, std::ios::binary);
			ifs)
		{
			ifs.exceptions(std::ios::badbit | std::ios::failbit);
			ifs.read(
				reinterpret_cast<char*>(std::ranges::data(output)),
				static_cast<std::streamsize>(read_size)
			);
		}
		else throw std::runtime_error(
			"read_binary_file() couldn't open " + file_path.string()
		);
	}
	else throw std::runtime_error(
		"read_binary_file() failed to read " + file_path.string() +
		" - buffer size too small ("
		+ std::to_string(buffer_size) + " < " + std::to_string(read_size) + ")"
	);
}

// Throws on failure.
template <fgl::traits::byte_type T = std::byte> [[nodiscard]]
std::vector<T> read_binary_file(const std::filesystem::path& file_path)
{
	const std::size_t file_size{ get_file_size(file_path) };
	std::vector<T> byte_buffer(file_size, T{});
	read_binary_file(file_path, byte_buffer, file_size);
	return byte_buffer;
}

/*Best effort. No information about the error will be provided.
If known_file_size is `0`, the file size will be obtained from std::filesystem.
The size of the output range must be >= the size of the file.*/
template <std::ranges::contiguous_range T_range>
requires fgl::traits::byte_type<typename T_range::value_type>
bool read_binary_file_noexcept(
	const std::filesystem::path& file_path,
	T_range& output,
	const std::size_t known_file_size = 0)
noexcept try
{
	read_binary_file(file_path, output, known_file_size);
	return true;
}
catch (...) { return false; }

// Best effort. No information about the error will be provided.
template <fgl::traits::byte_type T = std::byte> [[nodiscard]]
std::optional<std::vector<T>>
read_binary_file_noexcept(const std::filesystem::path& file_path)
noexcept try
{
	return read_binary_file<T>(file_path);
}
catch (...) { return std::nullopt; }

/*Throws on failure.
If length is `0`, the size of the input range will be used*/
template <std::ranges::contiguous_range T_range>
requires fgl::traits::byte_type<typename T_range::value_type>
void write_binary_file(
	const std::filesystem::path& file_path,
	const T_range& input,
	const std::size_t length = 0,
	const std::ios::openmode mode = std::ios::trunc)
{
	const std::streamsize number_of_bytes_to_write{
		(length > 0) ? static_cast<std::streamsize>(length)
		: static_cast<std::streamsize>(std::ranges::size(input))
	};
	if (std::ofstream ofs(file_path, std::ios::binary | mode);
		ofs)
	{
		ofs.exceptions(std::ios::badbit | std::ios::failbit);
		ofs.write(
			reinterpret_cast<const char*>(std::ranges::cdata(input)),
			number_of_bytes_to_write
		);
	}
	else throw std::runtime_error(
		"write_binary_file() couldn't open " + file_path.string()
	);
}

/*Best effort. No information about the error will be provided.
If length is `0`, the size of the input range will be used.*/
template <std::ranges::contiguous_range T_range>
requires fgl::traits::byte_type<typename T_range::value_type>
bool write_binary_file_noexcept(
	const std::filesystem::path& file_path,
	const T_range& input,
	const std::size_t length = 0,
	const std::ios::openmode mode = std::ios::trunc)
noexcept try
{
	write_binary_file(file_path, input, length, mode);
	return true;
}
catch (...) { return false; }

}// namespace fgl

#endif // FGL_UTILITY_BINARY_FILES_HPP_INCLUDED
