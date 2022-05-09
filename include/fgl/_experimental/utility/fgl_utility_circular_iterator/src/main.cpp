#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE

#define FGL_SHORT_MACROS
#include <fgl/debug/constexpr_assert.hpp>


#include <fgl/utility/circular_iterator.hpp>

#ifdef NDEBUG
	#error NDEBUG must not be defined for tests because they rely on assertions
#endif // NDEBUG

#include <array>

constexpr std::array<long long, 10> buffer{};
constexpr fgl::circular_iterator c_iter{buffer.begin(), buffer.end()};

constexpr unsigned int mult{ 2 };

constexpr bool test_ctors()
{
	fgl::circular_iterator ci1(buffer.begin(), buffer.end(), buffer.begin());
	fgl::circular_iterator ci2(buffer.begin(), buffer.end());
	fgl::circular_iterator ci3(buffer);
	constexpr_assert(&*ci1 == buffer.begin());
	constexpr_assert(ci1 == ci2);
	constexpr_assert(ci2 == ci3);
	// test infinite circular range helper
	constexpr auto ca{ fgl::make_circular_range_alias(buffer) };
	constexpr_assert(ca.begin() == ci1);
	constexpr_assert(
		(std::same_as<decltype(ca.end()), std::unreachable_sentinel_t>)
	);
	return true;
}


constexpr bool test_iteration_increment()
{
	fgl::circular_iterator iter{buffer.begin(), buffer.end()};
	for (unsigned int i{}; i < buffer.size() * mult; ++i, ++iter)
	{
		const auto expected{ &buffer[i % buffer.size()] };
		// test prefix operator++
		constexpr_assert(&*iter == expected);
		// test difference_type arithmetic
		constexpr_assert(&*(c_iter + i) == expected);
		auto c_iter_copy{ c_iter };
		constexpr_assert(&*(c_iter_copy += i) == expected);
	}
	return true;
}

constexpr bool test_iteration_decrement()
{
	fgl::circular_iterator iter{buffer.begin(), buffer.end()};
	auto cursor{ buffer.begin() };
	for (unsigned int i{0}; i < buffer.size() * mult; ++i, --iter, --cursor)
	{
		if (cursor < buffer.begin())
			cursor = buffer.end() - 1;
		const auto expected{ &*cursor };

		// test prefix operator--
		constexpr_assert(&*iter == expected);
		// test difference_type arithmetic operator
		constexpr_assert(&*(c_iter - i) == expected);
		auto c_iter_copy{ c_iter };
		constexpr_assert(&*(c_iter_copy -= i) == expected);
	}
	return true;
}

constexpr bool test_iteration_index()
{
	fgl::circular_iterator iter{buffer.begin(), buffer.end()};
	using dif_t = decltype(iter)::difference_type;
	auto cursor{ buffer.begin()};
	for (
		dif_t i{ static_cast<dif_t>(buffer.size()) * mult * -1 };
		i < static_cast<dif_t>(buffer.size())  * mult;
		++i, ++cursor)
	{
		if (cursor == buffer.end())
			cursor = buffer.begin();
		const auto expected{ &*cursor };
		const auto result{ &iter[i] };
		constexpr_assert(result == expected);
	}
	return true;
}


constexpr bool test_iteration()
{
	constexpr_assert(test_iteration_increment());
	constexpr_assert(test_iteration_decrement());
	constexpr_assert(test_iteration_index());
	return true;
}

constexpr bool test_arithmetic()
{
	fgl::circular_iterator iter{buffer.begin(), buffer.end()};
	constexpr_assert(iter + 0 == iter);
	constexpr_assert(iter - 0 == iter);
	constexpr_assert(&*(iter + 1) == buffer.cbegin() + 1);
	constexpr_assert(&*(iter - 1) == buffer.cend() - 1);
	constexpr_assert(iter + 1 == 1 + iter); // cumulative
	// wrapping
	constexpr_assert(iter + buffer.size() == iter);
	constexpr_assert(iter - buffer.size() == iter);
	constexpr_assert(iter + buffer.size() * mult == iter);
	constexpr_assert(iter - buffer.size() * mult == iter);
	constexpr_assert(iter + buffer.size() * mult * mult == iter);
	constexpr_assert(iter - buffer.size() * mult * mult == iter);
	// differential wrapping
	const auto iter2{ iter + 2 };
	constexpr_assert(iter2 - iter == 2);
	constexpr_assert(iter - iter2 == 8);
	constexpr_assert(iter2 + 8 == iter);
	constexpr_assert(iter - 8 == iter2);
	return true;
}


int main()
{
	static_assert(test_iteration());
	static_assert(test_arithmetic());

	return EXIT_SUCCESS;
}
