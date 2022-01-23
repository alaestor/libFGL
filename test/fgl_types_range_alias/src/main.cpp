#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE
#include <ranges>
#include <concepts>

#include <fgl/types/range_alias.hpp>

#define FGL_SHORT_MACROS
#include <fgl/debug/constexpr_assert.hpp>
#include <fgl/types/traits.hpp> // is_nonconst_ref

#ifdef NDEBUG
	#error NDEBUG must not be defined for tests because they rely on assertions
#endif // NDEBUG

int main()
{
	[]() consteval noexcept -> void
	{
		using fgl::range_alias;
		using fgl::traits::is_nonconst_ref;

		constexpr int len{ 10 };
		int a[len]{ 0 };
		int* const ptr = &a[0];

		const range_alias r(ptr, ptr+len);
		static_assert(std::ranges::common_range<decltype(r)>);
		static_assert(std::ranges::forward_range<decltype(r)>);
		static_assert(std::ranges::contiguous_range<decltype(r)>);

		for (int i{ 0 }; auto& n : r)
		{
			constexpr_assert(&n == &a[i++]);
			static_assert(is_nonconst_ref<decltype(n)>);
		}

		const auto r2{ fgl::make_range(ptr, len) };
		static_assert(std::same_as<decltype(r2), decltype(r)>);
		constexpr_assert(r.begin() == r2.begin());
		constexpr_assert(r.end() == r2.end());
	}();

	// constexpr
	static constexpr int arr[3]{ 0 };
	static constexpr auto r{ fgl::make_range(&arr[0], 3) };
	static_assert(std::ranges::common_range<decltype(r)>);
	static_assert(std::ranges::forward_range<decltype(r)>);
	static_assert(std::ranges::contiguous_range<decltype(r)>);

	return EXIT_SUCCESS;
}
