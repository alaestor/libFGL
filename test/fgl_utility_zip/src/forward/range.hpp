#pragma once
#ifndef FGL_TEST_ZIP_FORWARD_RANGE_HPP_INCLUDED
#define FGL_TEST_ZIP_FORWARD_RANGE_HPP_INCLUDED

#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE, size_t
#include <iterator> // forward_iterator
#include <ranges>
#include <algorithm>

#include <fgl/types/range_wrapper.hpp>
#include <fgl/utility/zip.hpp>

#define FGL_SHORT_MACROS
#include <fgl/debug./constexpr_assert.hpp>
#include <fgl/types/traits.hpp>

using fgl::traits::is_const_ref, fgl::traits::is_nonconst_ref;

#include "../types.hpp"

namespace forward::range {

using
	fgl::range_wrapper,
	fgl::zip,
	fgl::czip,
	fgl::internal::forward_zip_iterator;

using
	std::forward_iterator,
	std::ranges::forward_range;

using namespace container_and_range_types;

namespace globals {
array_t array{ 0,1,2,3,4,5,6,7 };
map_t map{ {0,'a'}, {1,'b'}, {3,'c'}, {4,'d'} };
list_t list{ 0,1,2,3,4,5,6,7,8,9 };
vector_t vector{ 'a','b','c','d','e','f' };
deque_t deque{ 0,1,2,3,4,5,6 };

const std::size_t smallest_length{
	std::min(
		{
			array.size(),
			map.size(),
			list.size(),
			vector.size(),
			deque.size()
		}
	)
};
}// namespace globals

consteval bool zip_rvalues()
{

	using rval_begin_iter_t = decltype(std::begin(array_t{1,2,3,4,5,6,7}));
	using zip_it_begin_t = forward_zip_iterator<rval_begin_iter_t>;
	static_assert(forward_iterator<zip_it_begin_t>);

	using zip_sentinel_t = typename zip_it_begin_t::difference_type;
	static_assert(std::sentinel_for<zip_sentinel_t, zip_it_begin_t>);

	using zip_expected_type = range_wrapper<zip_it_begin_t, zip_sentinel_t>;
	static_assert(forward_range<zip_expected_type>);

	auto zipped{ zip(array_t{0,1,2,3,4,5,6,7}) };
	static_assert(std::is_same_v<decltype(zipped), zip_expected_type>);

	return true;
}

bool zip_expected_behavior()
{
	using namespace globals;

	using zip_it_begin_t =
		forward_zip_iterator
		<
			array_begin_it_t,
			map_begin_it_t,
			list_begin_it_t,
			vector_begin_it_t,
			deque_begin_it_t
		>;
	static_assert(forward_iterator<zip_it_begin_t>);

	using zip_sentinel_t = typename zip_it_begin_t::difference_type;
	static_assert(std::sentinel_for<zip_sentinel_t, zip_it_begin_t>);

	using zip_expected_type = range_wrapper<zip_it_begin_t, zip_sentinel_t>;
	static_assert(forward_range<zip_expected_type>);

	auto zipped{ zip(array, map, list, vector, deque) };
	static_assert(std::is_same_v<decltype(zipped), zip_expected_type>);

	static_assert(std::is_same_v<decltype(zipped.begin()), zip_it_begin_t>);
	const zip_it_begin_t begin_zip_it{
		array.begin(), map.begin(), list.begin(), vector.begin(), deque.begin()
	};
	#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	constexpr_assert(zipped.begin() == begin_zip_it);
	#pragma GCC diagnostic pop // ignored "-Wdeprecated-declarations"

	static_assert(std::is_same_v<decltype(zipped.end()), zip_sentinel_t>);
	const zip_sentinel_t end_zip_it{
		static_cast<zip_sentinel_t>(std::min({
			array.size(), map.size(), list.size(), vector.size(), deque.size()
		}))
	};
	constexpr_assert(zipped.end() == end_zip_it);

	array_begin_it_t array_it{ array.begin() };
	map_begin_it_t map_it{ map.begin() };
	list_begin_it_t list_it{ list.begin() };
	vector_begin_it_t vector_it{ vector.begin() };
	deque_begin_it_t deque_it{ deque.begin() };

	std::size_t loop_count{ 0 };
	for (const auto& [array_r, map_r, list_r, vector_r, deque_r] : zipped)
	{
		static_assert(!std::is_const_v<array_t>);
		static_assert(is_nonconst_ref<decltype(array_r)>);
		constexpr_assert(&array_r == &*array_it++);

		static_assert(!std::is_const_v<map_t>);
		static_assert(is_nonconst_ref<decltype(map_r)>);
		const auto& [map_k, map_v]{ map_r };
		const auto& [map_it_k, map_it_v]{ *map_it++ };
		constexpr_assert(map_k == map_it_k);
		constexpr_assert(&map_v == &map_it_v);

		static_assert(std::is_const_v<list_t>);
		static_assert(is_const_ref<decltype(list_r)>);
		constexpr_assert(&list_r == &*list_it++);

		static_assert(std::is_const_v<vector_t>);
		static_assert(is_const_ref<decltype(vector_r)>);
		constexpr_assert(&vector_r == &*vector_it++);

		static_assert(!std::is_const_v<deque_t>);
		static_assert(is_nonconst_ref<decltype(deque_r)>);
		constexpr_assert(&deque_r == &*deque_it++);

		++loop_count;
	}
	constexpr_assert(loop_count == smallest_length);

	return true;
}

/// TODO

/* bool czip_expected_behavior()
{
	using namespace globals;

	using czip_it_begin_t =
		forward_zip_iterator
		<
			array_cbegin_it_t,
			map_cbegin_it_t,
			list_cbegin_it_t,
			vector_cbegin_it_t,
			deque_cbegin_it_t
		>;
	static_assert(forward_iterator<czip_it_begin_t>);

	using czip_it_end_t =
		forward_zip_iterator
		<
			array_cend_it_t,
			map_cend_it_t,
			list_cend_it_t,
			vector_cend_it_t,
			deque_cend_it_t
		>;
	static_assert(forward_iterator<czip_it_end_t>);
	static_assert(std::sentinel_for<czip_it_end_t, czip_it_begin_t>);

	using czip_expected_type = range_wrapper<czip_it_begin_t, czip_it_end_t>;
	static_assert(forward_range<czip_expected_type>);

	auto zipped{ czip(array, map, list, vector, deque) };
	static_assert(std::is_same_v<decltype(zipped), czip_expected_type>);

	static_assert(std::is_same_v<decltype(zipped.begin()), czip_it_begin_t>);
	const czip_it_begin_t begin_zip_it{
		array.cbegin(),
		map.cbegin(),
		list.cbegin(),
		vector.cbegin(),
		deque.cbegin()
	};
	assert(zipped.begin() == begin_zip_it);

	static_assert(std::is_same_v<decltype(zipped.end()), czip_it_end_t>);
	const czip_it_end_t end_zip_it{
		array.cend(),
		map.cend(),
		list.cend(),
		vector.cend(),
		deque.cend()
	};
	assert(zipped.end() == end_zip_it);

	array_cbegin_it_t array_it{ array.cbegin() };
	map_cbegin_it_t map_it{ map.cbegin() };
	list_cbegin_it_t list_it{ list.cbegin() };
	vector_cbegin_it_t vector_it{ vector.cbegin() };
	deque_cbegin_it_t deque_it{ deque.cbegin() };

	std::size_t loop_count{ 0 };
	for (const auto& [array_r, map_r, list_r, vector_r, deque_r] : zipped)
	{
		static_assert(!std::is_const_v<array_t>);
		static_assert(is_const_ref<decltype(array_r)>);
		assert(&array_r == &*array_it++);

		static_assert(!std::is_const_v<map_t>);
		static_assert(is_const_ref<decltype(map_r)>);
		const auto& [map_k, map_v]{ map_r };
		const auto& [map_it_k, map_it_v]{ *map_it++ };
		assert(map_k == map_it_k);
		assert(&map_v == &map_it_v);

		static_assert(std::is_const_v<list_t>);
		static_assert(is_const_ref<decltype(list_r)>);
		assert(&list_r == &*list_it++);

		static_assert(std::is_const_v<vector_t>);
		static_assert(is_const_ref<decltype(vector_r)>);
		assert(&vector_r == &*vector_it++);

		static_assert(!std::is_const_v<deque_t>);
		static_assert(is_const_ref<decltype(deque_r)>);
		assert(&deque_r == &*deque_it++);

		++loop_count;
	}
	assert(loop_count == smallest_length);

	return true;
} */

bool tests()
{
	assert(zip_expected_behavior());
	assert(zip_rvalues());
	//assert(czip_expected_behavior());
	return true;
}

} // namespace forward::range

#endif // FGL_TEST_ZIP_FORWARD_RANGE_HPP_INCLUDED
