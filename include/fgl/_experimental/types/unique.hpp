#pragma once
#ifndef FGL_TYPES_UNIQUE_HPP_INCLUDED
#define FGL_TYPES_UNIQUE_HPP_INCLUDED

/// QUICK-START GUIDE
/*
	A wrapper which creates a globally unique type for each instanciation.

	fgl::pattern::unique<int> a;
	fgl::pattern::unique<int> b;

	a.object = 3;
	b.object = 3;

	a == b; // ERROR `decltype(a)` and `decltype(b)` are different types

	a.object == b.object // OK
*/
/// EXAMPLE PROGRAM (using singleton)
/*
#include <fgl/pattern/singleton.hpp>
#include <fgl/pattern/unique.hpp>

using fgl::pattern::singleton, fgl::pattern::unique;
using unique_obj1_t = singleton<unique<internal::obj_singleton>>;
using unique_obj2_t = singleton<unique<internal::obj_singleton>>;

int main()
{
	static_assert(!std::same_as<unique_obj1_t, unique_obj2_t>);

	unique_obj1_t::instance().object.data = 1;
	unique_obj2_t::instance().object.data = 2;
	assert(unique_obj1_t::instance().object.data == 1);
	assert(unique_obj2_t::instance().object.data == 2);

	return EXIT_SUCCESS;
}
*/

#include <utility>

namespace fgl {

// A unique type template: each instanciation is a new type.
template <class T, typename T_GUID = decltype([](){})>
struct unique
{
	using id = T_GUID;
    T object;

	[[nodiscard]] explicit constexpr
	unique(auto&& ... args)
	: object(std::forward<decltype(args)>(args)...)
	{}

	[[nodiscard]] constexpr operator T() const noexcept
	{ return object; }

    [[nodiscard]] constexpr T& operator()() noexcept
    { return object; }

    [[nodiscard]] constexpr const T& operator()() const noexcept
    { return object; }
};

} // namespace fgl

#endif // FGL_TYPES_UNIQUE_HPP_INCLUDED
