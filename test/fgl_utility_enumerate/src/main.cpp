#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE
#include <array>

#define FGL_SHORT_MACROS
#include <fgl/debug/constexpr_assert.hpp>
#include <fgl/types/traits.hpp>

#include <fgl/utility/enumerate.hpp>

#ifdef NDEBUG
	#error NDEBUG must not be defined for tests because they rely on assertions
#endif // NDEBUG

#include <ranges> // forward_range


int main()
{
	[]() consteval -> void
	{
		using namespace fgl::traits;
		using fgl::enumerate, fgl::cenumerate;

		/// TODO once libc bug is fixed, rework test with a second type
		// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=100639

		std::array a{1,2,3,4};
		using T1 = uint_fast32_t;

		for (const auto& [i,v] : enumerate<T1>(a))
		{
			static_assert(std::is_same_v<std::remove_cvref_t<decltype(i)>, T1>);
			static_assert(is_nonconst_ref<decltype(v)>);
			constexpr_assert(&v == &a[i]);
		}

		for (const auto& [i,v] : cenumerate<T1>(a))
		{
			static_assert(is_const_ref<decltype(v)>);
			constexpr_assert(&v == &a[i]);
		}
	}();

	return EXIT_SUCCESS;
}
