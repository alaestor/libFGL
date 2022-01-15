#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE
#include <array>
#include <algorithm> // equal
#include <cassert>

#include <stdexcept>
#include <limits>
#include <iostream>

#include <fgl/utility/make_byte_array.hpp>

#include <fgl/io/binary_files.hpp>

#ifdef NDEBUG
	#error NDEBUG must not be defined for tests because they rely on assertions
#endif // NDEBUG

using namespace fgl;

constexpr auto binary_data{ make_byte_array("\xFF im a binary file \xAA") };
const std::filesystem::path nonexistent_file_path{ "__path/doesnt/exist" };

bool test_write_file(const std::filesystem::path& file_path)
{
	write_binary_file(file_path, binary_data);
	return true;
}

bool test_get_file_size(const std::filesystem::path& file_path)
{
	const std::size_t size{ get_file_size(file_path) };
	assert(size == binary_data.size());
	return true;
}

bool test_read_file(const std::filesystem::path& file_path)
{
	auto file_contents{ read_binary_file(file_path) };
	assert(file_contents.size() == binary_data.size());
	assert(std::ranges::equal(file_contents, binary_data));
	return true;
}

consteval bool test_noexcept_variants_are_specified()
{
	static_assert(noexcept(get_file_size_noexcept));
	static_assert(noexcept(read_binary_file_noexcept<std::byte>));
	static_assert(noexcept(write_binary_file_noexcept<decltype(binary_data)>));
	return true;
}

bool test_write_file_noexcept()
{
	// should fail
	assert(!write_binary_file_noexcept(nonexistent_file_path, binary_data));
	return true;
}

bool test_get_file_size_noexcept()
{
	// should fail
	const std::size_t size{ get_file_size_noexcept(nonexistent_file_path) };
	assert(size == std::numeric_limits<std::size_t>::max());
	return true;
}

bool test_read_file_noexcept()
{
	// should fail
	auto file_contents{ read_binary_file_noexcept(nonexistent_file_path) };
	assert(!file_contents);
	return true;
}

/// README
/*
	This test needs to write and read from to a file on disk,
	so it expects the path to the file to be passed as argv[1].
*/

int main(const int argc, const char* const argv[]) try
{
	assert(argc > 1);
	const std::filesystem::path file_path{ argv[1] };

	assert(test_write_file(file_path));
	assert(test_get_file_size(file_path));
	assert(test_read_file(file_path));

	static_assert(test_noexcept_variants_are_specified());

	assert(test_write_file_noexcept());
	assert(test_get_file_size_noexcept());
	assert(test_read_file_noexcept());

	return EXIT_SUCCESS;
}
catch (const std::exception& e)
{
	std::cerr << "caught exception " << e.what() << std::endl;
	std::abort();
}
catch (...)
{
	std::cerr << "caught unknown exception" << std::endl;
	std::abort();
}
