#pragma once
#ifndef FGL_RANDOM_HPP_INCLUDED
#define FGL_RANDOM_HPP_INCLUDED

/// QUICK-START GUIDE
/* Syntax: required [optional]

    fgl::random<[type], [distribution], [generator], [table size]>
        my_rng([min value], [max value], [seed]);

    type var = my_rng(); // returns `type` between `min` and `max` (inclusive)

	template:
	`type` numeric datatype. supports floating-point. [default: uint_fast64_t]
	`distribution` statistical distribution [uniform int/real distribution]
	`generator` pseudo-random number generator [mt19937_64]
	`table size` count of outputs to shuffle (helps improve randomness) [8]

	constructor:
	`min value` minimum (inclusive) value that can be generated [min of type]
	`max value` maximum (inclusive) value that can be generated [max of type]
	`seed` to seed the shuffled generator [std::random_device{}()]
*/
/// EXAMPLE PROGRAM
/*
	#include <iostream> // cout, endl
	#include <fgl/utility/random.hpp>

	int main()
	{
		using std::cout, std::endl, fgl::random;

		// defualts: uint_fast64_t, between type min and max, uniform dist.
		random rng_a;
		cout << "between min and max uint_fast64_t: " << rng_a() << endl;

		random<int> rng_b(-9, 9);
		cout << "between -9 and 9 int: " << rng_b() << endl;

		random<uint_least8_t> rng_c;
		cout
			<< "between min and max uint_least8_t: "
			<< static_cast<unsigned int>(rng_c())
			<< endl;
		// typecast because underlying 8bit type is often a char

		random<float> rng_d(3.14f, 69.69f);
		cout << "between 3.14 and 69.69 float: " << rng_d() << endl;

		// changing distribution
		random<double, std::normal_distribution<double>> rng_e;
		cout << "between min and max double, normal dist: " << rng_e() << endl;
	}
*/
/// EXAMPLE OUTPUT
/*
between min and max uint_fast64_t: 6909273070770195903
between between -9 and 9 int: -3
between min and max uint_least8_t: 156
between 3.14 and 69.69 float: 28.5469
between min and max double, normal dist: -inf
*/

#include <cstdint> // uint_fast64_t
#include <concepts> // integral, floating_point
#include <limits> // numeric_limits
#include <random>

#include "../types/traits.hpp" // numeric_type

namespace fgl {

/// IMPORTANT NOTE this header uses sadface initializer syntax

/*A callable object that can generate pseudo-random numbers of type T along a
T_distribution, using T_generator, between values min and max (inclusive).
Call the object to generate a number. For example:
`random rng(1,5); int var = rng();`*/
template
<
	fgl::traits::numeric_type T = uint_fast64_t,
	class T_distribution =
		std::conditional
		<
			std::integral<T>,
			std::uniform_int_distribution<T>,
			std::uniform_real_distribution<T>
		>::type,
	class T_generator = std::mt19937_64,
	const std::size_t T_table_size = 8
>
struct random
{
	std::shuffle_order_engine<T_generator, T_table_size> engine;
	T_distribution distribution;

	[[nodiscard]] explicit random(
		const T min = std::numeric_limits<T>::min(),
		const T max = std::numeric_limits<T>::max(),
		const std::random_device::result_type seed = (std::random_device{}())
	):
		engine(seed),
		distribution(min, max)
	{}

	// generates a random T value in accordance with the fgl::random object.
	[[nodiscard]] T operator()()
	{ return distribution(engine); }
};

}// namespace fgl

#endif // RANDOMINT_HPP_INCLUDED
