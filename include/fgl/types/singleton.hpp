#pragma once
#ifndef FGL_TYPES_SINGLETON_HPP_INCLUDED
#define FGL_TYPES_SINGLETON_HPP_INCLUDED

/// QUICK-START GUIDE / EXAMPLE PROGRAM
/*
	#include <cassert>
	#include <fgl/types/singleton.hpp>

	class obj_impl
	{
		FGL_PATTERN_SINGLETON_BOILERPLATE(obj); // optional
		public:
		int data{ 0 };
	};
	using obj = fgl::singleton<obj_impl>;

	void f1()
	{
		obj::instance().data = 1337;
	}

	int main()
	{
		f();
		assert(obj::instance().data == 1337);
	}
*/
/*
Type-based static singleton pattern template.

FGL singletons don't require the the template type to be a typical
"singleton-exclusive" object (*Normally, types intended to be used as
singletons are exclusively singletons and only have a private constructor and
delete the copy and move constructors, along with the assignment operator*).
Any type that satisfies the standard `std::default_initializable` concept
can be used with the singleton template. For example, `fgl::singleton<int>`
is a valid singleton whose `instance()` method will return an `int&` to a
static `int`.

Note: Despite the requirement that `T` satisfy the `std::default_initializable`
concept, the singleton template isn't constrained by it due to
"incomplete type" issues which would complicate object definitions.
*/

#include <type_traits>

#ifndef FGL_SINGLETON_BOILERPLATE
	#define FGL_SINGLETON_BOILERPLATE(class_name) \
		friend class fgl::singleton<class_name>;\
		class_name() = default;\
		class_name(const class_name&) = delete;\
		class_name(const class_name&&) = delete;\
		void operator=(const class_name&) = delete
#else
	#error FGL_SINGLETON_BOILERPLATE already defined
#endif // FGL_SINGLETON_BOILERPLATE

namespace fgl {

template <typename T>
class singleton final
{
	singleton() = delete;
	singleton(const singleton&) = delete;
	~singleton() = delete;
	void operator=(const singleton&) = delete;

public:
	static T& instance() noexcept
	{
		static T instance{};
		return instance;
	}
};

}// namespace fgl

#endif // FGL_TYPES_SINGLETON_HPP_INCLUDED
