#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE

#include <fgl/types/traits.hpp>

#define FGL_SHORT_MACROS
#include <fgl/debug/constexpr_assert.hpp>

#ifdef NDEBUG
	#error NDEBUG must not be defined for tests because they rely on assertions
#endif // NDEBUG

consteval bool test_ref_traits()
{
	using
		fgl::traits::is_const_ref,
		fgl::traits::is_nonconst_ref;

	using underlying_t = double;
	using ref_t = underlying_t&;
	using cref_t = const underlying_t&;

	static_assert(!is_const_ref<underlying_t>);
	static_assert(!is_const_ref<ref_t>);
	static_assert(is_const_ref<cref_t>);

	static_assert(!is_nonconst_ref<underlying_t>);
	static_assert(is_nonconst_ref<ref_t>);
	static_assert(!is_nonconst_ref<cref_t>);
	return true;
}

consteval bool test_remove_cvptr_t()
{
	using fgl::traits::remove_cvptr_t;
	using T = double;
	static_assert(std::same_as<remove_cvptr_t<T* const>, T>);
	static_assert(std::same_as<remove_cvptr_t<const T* const>, T>);
	static_assert(std::same_as<remove_cvptr_t<T* volatile>, T>);
	static_assert(std::same_as<remove_cvptr_t<const volatile T* const>, T>);
	return true;
}

namespace test_remove_all_pointers_globals {
using fgl::traits::remove_all_pointers_t;
template <typename T, typename T_ptr>
constexpr bool test{ std::same_as<T, remove_all_pointers_t<T_ptr>> };
} // namespace test_remove_all_pointers_globals
consteval bool test_remove_all_pointers()
{
	using namespace test_remove_all_pointers_globals;

	using T = double;
	static_assert(test<T, T*>);
	static_assert(test<T, T**>);
	static_assert(test<T, T**************>);
	static_assert(test<T, T* const*>);
	static_assert(test<T, T* const* volatile*>);
	static_assert(test<T, T* const* volatile* const volatile*>);
	static_assert(test<T[], T(* const* volatile* const volatile*)[]>);
	static_assert(test<const T, const T* const* volatile* const volatile*>);
	static_assert(!test<T, const T* const* volatile* const volatile*>);
	static_assert(!test<const T, T* const* volatile* const volatile*>);

	return true;
}

consteval bool test_byte_type()
{
	using fgl::traits::byte_type;

	static_assert(byte_type<std::byte>);
	static_assert(byte_type<char>);
	static_assert(byte_type<unsigned char>);

	static_assert(!byte_type<void>);
	static_assert(!byte_type<signed char>);
	static_assert(!byte_type<int>);
	static_assert(!byte_type<short>);
	static_assert(!byte_type<double>);

	return true;
}

template <typename T>
consteval bool for_all_cvptr_permutations_of()
{
	using fgl::traits::pointer_to_byte;
	static_assert(pointer_to_byte<T*>);
	static_assert(pointer_to_byte<const T*>);
	static_assert(pointer_to_byte<volatile T*>);
	static_assert(pointer_to_byte<const volatile T*>);
	static_assert(pointer_to_byte<T* const>);
	static_assert(pointer_to_byte<T* volatile>);
	static_assert(pointer_to_byte<T* const volatile>);
	static_assert(pointer_to_byte<const T* const>);
	static_assert(pointer_to_byte<volatile T* volatile>);
	static_assert(pointer_to_byte<const volatile T* const volatile>);
	return true;
}

consteval bool test_pointer_to_byte()
{
	static_assert(for_all_cvptr_permutations_of<std::byte>());
	static_assert(for_all_cvptr_permutations_of<char>());
	static_assert(for_all_cvptr_permutations_of<unsigned char>());
	static_assert(for_all_cvptr_permutations_of<void>());

	using fgl::traits::pointer_to_byte;
	static_assert(!pointer_to_byte<signed char>);
	static_assert(!pointer_to_byte<int*>);
	static_assert(!pointer_to_byte<char**>);
	static_assert(!pointer_to_byte<double*>);

	return true;
}

consteval bool test_pointer_traits()
{
	static_assert(test_remove_cvptr_t());
	static_assert(test_remove_all_pointers());
	static_assert(test_byte_type());
	static_assert(test_pointer_to_byte());

	return true;
}

int main()
{
	static_assert(test_ref_traits());
	static_assert(test_pointer_traits());
	return EXIT_SUCCESS;
}
