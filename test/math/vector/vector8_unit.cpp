/*! @file   vector5_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Test unit for size 8 vectors.
 *  @date   27-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

// Include: Pulsar
#include "pulsar/mathematics.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Include: C++
#include <numeric>
#include <random>

// Pulsar
namespace pul
{
	// Math
	namespace math
	{
		/// MATH: Col Vector8 Tests
		TEST_CASE("MathColInt32Vec8Test1")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			// Constructor
			const int32_t i1 = r(gen), i2 = r(gen), i3 = r(gen),
										i4 = r(gen), i5 = r(gen), i6 = r(gen),
										i7 = r(gen), i8 = r(gen);
			cvec<int32_t, 8, SIMD_UNALIGNED> v1 = { i1, i2, i3, i4, i5, i6, i7, i8 };
			REQUIRE(i1 == v1[0]);
			REQUIRE(i2 == v1[1]);
			REQUIRE(i3 == v1[2]);
			REQUIRE(i4 == v1[3]);
			REQUIRE(i5 == v1[4]);
			REQUIRE(i6 == v1[5]);
			REQUIRE(i7 == v1[6]);
			REQUIRE(i8 == v1[7]);
			// Assignment
			cvec<int32_t, 8, SIMD_ALIGNED> v2;
			v2 = v1;
			REQUIRE(i1 == v2[0]);
			REQUIRE(i2 == v2[1]);
			REQUIRE(i3 == v2[2]);
			REQUIRE(i4 == v2[3]);
			REQUIRE(i5 == v2[4]);
			REQUIRE(i6 == v2[5]);
			REQUIRE(i7 == v2[6]);
			REQUIRE(i8 == v2[7]);
		}
		TEST_CASE("MathColInt32Vec8Test2")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			cvec<int32_t, 8, SIMD_UNALIGNED> v1 = {
				r(gen),
				r(gen),
				r(gen),
				r(gen),
				r(gen),
				r(gen),
				r(gen),
				r(gen)
			};
			cvec<int32_t, 8, SIMD_ALIGNED> v2 = {
				r(gen),
				r(gen),
				r(gen),
				r(gen),
				r(gen),
				r(gen),
				r(gen),
				r(gen)
			};
			const int32_t rn = r(gen);
			// +
			auto rv					 = v1 + v2;
			REQUIRE(rv[0] == (v1[0] + v2[0]));
			REQUIRE(rv[1] == (v1[1] + v2[1]));
			REQUIRE(rv[2] == (v1[2] + v2[2]));
			REQUIRE(rv[3] == (v1[3] + v2[3]));
			REQUIRE(rv[4] == (v1[4] + v2[4]));
			REQUIRE(rv[5] == (v1[5] + v2[5]));
			REQUIRE(rv[6] == (v1[6] + v2[6]));
			REQUIRE(rv[7] == (v1[7] + v2[7]));
			// -
			rv = v1 - v2;
			REQUIRE(rv[0] == (v1[0] - v2[0]));
			REQUIRE(rv[1] == (v1[1] - v2[1]));
			REQUIRE(rv[2] == (v1[2] - v2[2]));
			REQUIRE(rv[3] == (v1[3] - v2[3]));
			REQUIRE(rv[4] == (v1[4] - v2[4]));
			REQUIRE(rv[5] == (v1[5] - v2[5]));
			REQUIRE(rv[6] == (v1[6] - v2[6]));
			REQUIRE(rv[7] == (v1[7] - v2[7]));
			// * (scalar)
			rv = v1 * rn;
			REQUIRE(rv[0] == (v1[0] * rn));
			REQUIRE(rv[1] == (v1[1] * rn));
			REQUIRE(rv[2] == (v1[2] * rn));
			REQUIRE(rv[3] == (v1[3] * rn));
			REQUIRE(rv[4] == (v1[4] * rn));
			REQUIRE(rv[5] == (v1[5] * rn));
			REQUIRE(rv[6] == (v1[6] * rn));
			REQUIRE(rv[7] == (v1[7] * rn));

			// / (scalar)
			rv = v1 / rn;
			REQUIRE(rv[0] == (v1[0] / rn));
			REQUIRE(rv[1] == (v1[1] / rn));
			REQUIRE(rv[2] == (v1[2] / rn));
			REQUIRE(rv[3] == (v1[3] / rn));
			REQUIRE(rv[4] == (v1[4] / rn));
			REQUIRE(rv[5] == (v1[5] / rn));
			REQUIRE(rv[6] == (v1[6] / rn));
			REQUIRE(rv[7] == (v1[7] / rn));
			// %
			rv = v1 % v2;
			REQUIRE(rv[0] == (v1[0] % v2[0]));
			REQUIRE(rv[1] == (v1[1] % v2[1]));
			REQUIRE(rv[2] == (v1[2] % v2[2]));
			REQUIRE(rv[3] == (v1[3] % v2[3]));
			REQUIRE(rv[4] == (v1[4] % v2[4]));
			REQUIRE(rv[5] == (v1[5] % v2[5]));
			REQUIRE(rv[6] == (v1[6] % v2[6]));
			REQUIRE(rv[7] == (v1[7] % v2[7]));
			// &
			rv = v1 & v2;
			REQUIRE(rv[0] == (v1[0] & v2[0]));
			REQUIRE(rv[1] == (v1[1] & v2[1]));
			REQUIRE(rv[2] == (v1[2] & v2[2]));
			REQUIRE(rv[3] == (v1[3] & v2[3]));
			REQUIRE(rv[4] == (v1[4] & v2[4]));
			REQUIRE(rv[5] == (v1[5] & v2[5]));
			REQUIRE(rv[6] == (v1[6] & v2[6]));
			REQUIRE(rv[7] == (v1[7] & v2[7]));
			// %
			rv = v1 | v2;
			REQUIRE(rv[0] == (v1[0] | v2[0]));
			REQUIRE(rv[1] == (v1[1] | v2[1]));
			REQUIRE(rv[2] == (v1[2] | v2[2]));
			REQUIRE(rv[3] == (v1[3] | v2[3]));
			REQUIRE(rv[4] == (v1[4] | v2[4]));
			REQUIRE(rv[5] == (v1[5] | v2[5]));
			REQUIRE(rv[6] == (v1[6] | v2[6]));
			REQUIRE(rv[7] == (v1[7] | v2[7]));
			// ^
			rv = v1 % v2;
			REQUIRE(rv[0] == v1[0] % v2[0]);
			REQUIRE(rv[1] == v1[1] % v2[1]);
			REQUIRE(rv[2] == v1[2] % v2[2]);
			REQUIRE(rv[3] == v1[3] % v2[3]);
			REQUIRE(rv[4] == v1[4] % v2[4]);
			REQUIRE(rv[5] == v1[5] % v2[5]);
			REQUIRE(rv[6] == v1[6] % v2[6]);
			REQUIRE(rv[7] == v1[7] % v2[7]);
			// ~
			rv = ~v1;
			REQUIRE(rv[0] == ~v1[0]);
			REQUIRE(rv[1] == ~v1[1]);
			REQUIRE(rv[2] == ~v1[2]);
			REQUIRE(rv[3] == ~v1[3]);
			REQUIRE(rv[4] == ~v1[4]);
			REQUIRE(rv[5] == ~v1[5]);
			REQUIRE(rv[6] == ~v1[6]);
			REQUIRE(rv[7] == ~v1[7]);
			// >>
			rv = v1 >> v2;
			REQUIRE(rv[0] == v1[0] >> v2[0]);
			REQUIRE(rv[1] == v1[1] >> v2[1]);
			REQUIRE(rv[2] == v1[2] >> v2[2]);
			REQUIRE(rv[3] == v1[3] >> v2[3]);
			REQUIRE(rv[4] == v1[4] >> v2[4]);
			REQUIRE(rv[5] == v1[5] >> v2[5]);
			REQUIRE(rv[6] == v1[6] >> v2[6]);
			REQUIRE(rv[7] == v1[7] >> v2[7]);
			// <<
			rv = v1 << v2;
			REQUIRE(rv[0] == v1[0] << v2[0]);
			REQUIRE(rv[1] == v1[1] << v2[1]);
			REQUIRE(rv[2] == v1[2] << v2[2]);
			REQUIRE(rv[3] == v1[3] << v2[3]);
			REQUIRE(rv[4] == v1[4] << v2[4]);
			REQUIRE(rv[5] == v1[5] << v2[5]);
			REQUIRE(rv[6] == v1[6] << v2[6]);
			REQUIRE(rv[7] == v1[7] << v2[7]);
			// ==
			bool beq = (v1 == v2);
			REQUIRE(beq == (v1[0] == v2[0]));
			REQUIRE(beq == (v1[1] == v2[1]));
			REQUIRE(beq == (v1[2] == v2[2]));
			REQUIRE(beq == (v1[3] == v2[3]));
			REQUIRE(beq == (v1[4] == v2[4]));
			REQUIRE(beq == (v1[5] == v2[5]));
			REQUIRE(beq == (v1[6] == v2[6]));
			REQUIRE(beq == (v1[7] == v2[7]));
			// vals_equal
			auto eq = (vals_equal(v1, v2));
			REQUIRE(eq[0] == (v1[0] == v2[0]));
			REQUIRE(eq[1] == (v1[1] == v2[1]));
			REQUIRE(eq[2] == (v1[2] == v2[2]));
			REQUIRE(eq[3] == (v1[3] == v2[3]));
			REQUIRE(eq[4] == (v1[4] == v2[4]));
			REQUIRE(eq[5] == (v1[5] == v2[5]));
			REQUIRE(eq[6] == (v1[6] == v2[6]));
			REQUIRE(eq[7] == (v1[7] == v2[7]));
			// vals_greater
			auto gq = (vals_greater(v1, v2));
			REQUIRE(gq[0] == (v1[0] > v2[0]));
			REQUIRE(gq[1] == (v1[1] > v2[1]));
			REQUIRE(gq[2] == (v1[2] > v2[2]));
			REQUIRE(gq[3] == (v1[3] > v2[3]));
			REQUIRE(gq[4] == (v1[4] > v2[4]));
			REQUIRE(gq[5] == (v1[5] > v2[5]));
			REQUIRE(gq[6] == (v1[6] > v2[6]));
			REQUIRE(gq[7] == (v1[7] > v2[7]));
			// vals_greater_equal
			auto geq = (vals_greater_equal(v1, v2));
			REQUIRE(geq[0] == (v1[0] >= v2[0]));
			REQUIRE(geq[1] == (v1[1] >= v2[1]));
			REQUIRE(geq[2] == (v1[2] >= v2[2]));
			REQUIRE(geq[3] == (v1[3] >= v2[3]));
			REQUIRE(geq[4] == (v1[4] >= v2[4]));
			REQUIRE(geq[5] == (v1[5] >= v2[5]));
			REQUIRE(geq[6] == (v1[6] >= v2[6]));
			REQUIRE(geq[7] == (v1[7] >= v2[7]));
			// vals_smaller
			auto sq = (vals_smaller(v1, v2));
			REQUIRE(sq[0] == (v1[0] < v2[0]));
			REQUIRE(sq[1] == (v1[1] < v2[1]));
			REQUIRE(sq[2] == (v1[2] < v2[2]));
			REQUIRE(sq[3] == (v1[3] < v2[3]));
			REQUIRE(sq[4] == (v1[4] < v2[4]));
			REQUIRE(sq[5] == (v1[5] < v2[5]));
			REQUIRE(sq[6] == (v1[6] < v2[6]));
			REQUIRE(sq[7] == (v1[7] < v2[7]));
			// vals_smaller_equal
			auto seq = (vals_smaller_equal(v1, v2));
			REQUIRE(seq[0] == (v1[0] <= v2[0]));
			REQUIRE(seq[1] == (v1[1] <= v2[1]));
			REQUIRE(seq[2] == (v1[2] <= v2[2]));
			REQUIRE(seq[3] == (v1[3] <= v2[3]));
			REQUIRE(seq[4] == (v1[4] <= v2[4]));
			REQUIRE(seq[5] == (v1[5] <= v2[5]));
			REQUIRE(seq[6] == (v1[6] <= v2[6]));
			REQUIRE(seq[7] == (v1[7] <= v2[7]));
			// vals_not_equal
			auto neq = (vals_not_equal(v1, v2));
			REQUIRE(neq[0] == (v1[0] != v2[0]));
			REQUIRE(neq[1] == (v1[1] != v2[1]));
			REQUIRE(neq[2] == (v1[2] != v2[2]));
			REQUIRE(neq[3] == (v1[3] != v2[3]));
			REQUIRE(neq[4] == (v1[4] != v2[4]));
			REQUIRE(neq[5] == (v1[5] != v2[5]));
			REQUIRE(neq[6] == (v1[6] != v2[6]));
			REQUIRE(neq[7] == (v1[7] != v2[7]));
		}

		/// MATH: Row Vector5 Tests
		TEST_CASE("MathRowInt32Vec8Test1")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			// Constructor
			const int32_t i1 = r(gen), i2 = r(gen), i3 = r(gen),
										i4 = r(gen), i5 = r(gen), i6 = r(gen),
										i7 = r(gen), i8 = r(gen);
			rvec<int32_t, 8, SIMD_UNALIGNED> v1 = { i1, i2, i3, i4, i5, i6, i7, i8 };
			REQUIRE(i1 == v1[0]);
			REQUIRE(i2 == v1[1]);
			REQUIRE(i3 == v1[2]);
			REQUIRE(i4 == v1[3]);
			REQUIRE(i5 == v1[4]);
			REQUIRE(i6 == v1[5]);
			REQUIRE(i7 == v1[6]);
			REQUIRE(i8 == v1[7]);
			// Assignment
			rvec<int32_t, 8, SIMD_ALIGNED> v2;
			v2 = v1;
			REQUIRE(i1 == v2[0]);
			REQUIRE(i2 == v2[1]);
			REQUIRE(i3 == v2[2]);
			REQUIRE(i4 == v2[3]);
			REQUIRE(i5 == v2[4]);
			REQUIRE(i6 == v2[5]);
			REQUIRE(i7 == v2[6]);
			REQUIRE(i8 == v2[7]);
		}
		TEST_CASE("MathRowInt32Vec8Test2")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			rvec<int32_t, 8, SIMD_UNALIGNED> v1 = {
				r(gen),
				r(gen),
				r(gen),
				r(gen),
				r(gen),
				r(gen),
				r(gen),
				r(gen)
			};
			rvec<int32_t, 8, SIMD_ALIGNED> v2 = {
				r(gen),
				r(gen),
				r(gen),
				r(gen),
				r(gen),
				r(gen),
				r(gen),
				r(gen)
			};
			const int32_t rn = r(gen);
			// +
			auto rv					 = v1 + v2;
			REQUIRE(rv[0] == (v1[0] + v2[0]));
			REQUIRE(rv[1] == (v1[1] + v2[1]));
			REQUIRE(rv[2] == (v1[2] + v2[2]));
			REQUIRE(rv[3] == (v1[3] + v2[3]));
			REQUIRE(rv[4] == (v1[4] + v2[4]));
			REQUIRE(rv[5] == (v1[5] + v2[5]));
			REQUIRE(rv[6] == (v1[6] + v2[6]));
			REQUIRE(rv[7] == (v1[7] + v2[7]));
			// -
			rv = v1 - v2;
			REQUIRE(rv[0] == (v1[0] - v2[0]));
			REQUIRE(rv[1] == (v1[1] - v2[1]));
			REQUIRE(rv[2] == (v1[2] - v2[2]));
			REQUIRE(rv[3] == (v1[3] - v2[3]));
			REQUIRE(rv[4] == (v1[4] - v2[4]));
			REQUIRE(rv[5] == (v1[5] - v2[5]));
			REQUIRE(rv[6] == (v1[6] - v2[6]));
			REQUIRE(rv[7] == (v1[7] - v2[7]));
			// * (scalar)
			rv = v1 * rn;
			REQUIRE(rv[0] == (v1[0] * rn));
			REQUIRE(rv[1] == (v1[1] * rn));
			REQUIRE(rv[2] == (v1[2] * rn));
			REQUIRE(rv[3] == (v1[3] * rn));
			REQUIRE(rv[4] == (v1[4] * rn));
			REQUIRE(rv[5] == (v1[5] * rn));
			REQUIRE(rv[6] == (v1[6] * rn));
			REQUIRE(rv[7] == (v1[7] * rn));

			// / (scalar)
			rv = v1 / rn;
			REQUIRE(rv[0] == (v1[0] / rn));
			REQUIRE(rv[1] == (v1[1] / rn));
			REQUIRE(rv[2] == (v1[2] / rn));
			REQUIRE(rv[3] == (v1[3] / rn));
			REQUIRE(rv[4] == (v1[4] / rn));
			REQUIRE(rv[5] == (v1[5] / rn));
			REQUIRE(rv[6] == (v1[6] / rn));
			REQUIRE(rv[7] == (v1[7] / rn));
			// %
			rv = v1 % v2;
			REQUIRE(rv[0] == (v1[0] % v2[0]));
			REQUIRE(rv[1] == (v1[1] % v2[1]));
			REQUIRE(rv[2] == (v1[2] % v2[2]));
			REQUIRE(rv[3] == (v1[3] % v2[3]));
			REQUIRE(rv[4] == (v1[4] % v2[4]));
			REQUIRE(rv[5] == (v1[5] % v2[5]));
			REQUIRE(rv[6] == (v1[6] % v2[6]));
			REQUIRE(rv[7] == (v1[7] % v2[7]));
			// &
			rv = v1 & v2;
			REQUIRE(rv[0] == (v1[0] & v2[0]));
			REQUIRE(rv[1] == (v1[1] & v2[1]));
			REQUIRE(rv[2] == (v1[2] & v2[2]));
			REQUIRE(rv[3] == (v1[3] & v2[3]));
			REQUIRE(rv[4] == (v1[4] & v2[4]));
			REQUIRE(rv[5] == (v1[5] & v2[5]));
			REQUIRE(rv[6] == (v1[6] & v2[6]));
			REQUIRE(rv[7] == (v1[7] & v2[7]));
			// %
			rv = v1 | v2;
			REQUIRE(rv[0] == (v1[0] | v2[0]));
			REQUIRE(rv[1] == (v1[1] | v2[1]));
			REQUIRE(rv[2] == (v1[2] | v2[2]));
			REQUIRE(rv[3] == (v1[3] | v2[3]));
			REQUIRE(rv[4] == (v1[4] | v2[4]));
			REQUIRE(rv[5] == (v1[5] | v2[5]));
			REQUIRE(rv[6] == (v1[6] | v2[6]));
			REQUIRE(rv[7] == (v1[7] | v2[7]));
			// ^
			rv = v1 % v2;
			REQUIRE(rv[0] == v1[0] % v2[0]);
			REQUIRE(rv[1] == v1[1] % v2[1]);
			REQUIRE(rv[2] == v1[2] % v2[2]);
			REQUIRE(rv[3] == v1[3] % v2[3]);
			REQUIRE(rv[4] == v1[4] % v2[4]);
			REQUIRE(rv[5] == v1[5] % v2[5]);
			REQUIRE(rv[6] == v1[6] % v2[6]);
			REQUIRE(rv[7] == v1[7] % v2[7]);
			// ~
			rv = ~v1;
			REQUIRE(rv[0] == ~v1[0]);
			REQUIRE(rv[1] == ~v1[1]);
			REQUIRE(rv[2] == ~v1[2]);
			REQUIRE(rv[3] == ~v1[3]);
			REQUIRE(rv[4] == ~v1[4]);
			REQUIRE(rv[5] == ~v1[5]);
			REQUIRE(rv[6] == ~v1[6]);
			REQUIRE(rv[7] == ~v1[7]);
			// >>
			rv = v1 >> v2;
			REQUIRE(rv[0] == v1[0] >> v2[0]);
			REQUIRE(rv[1] == v1[1] >> v2[1]);
			REQUIRE(rv[2] == v1[2] >> v2[2]);
			REQUIRE(rv[3] == v1[3] >> v2[3]);
			REQUIRE(rv[4] == v1[4] >> v2[4]);
			REQUIRE(rv[5] == v1[5] >> v2[5]);
			REQUIRE(rv[6] == v1[6] >> v2[6]);
			REQUIRE(rv[7] == v1[7] >> v2[7]);
			// <<
			rv = v1 << v2;
			REQUIRE(rv[0] == v1[0] << v2[0]);
			REQUIRE(rv[1] == v1[1] << v2[1]);
			REQUIRE(rv[2] == v1[2] << v2[2]);
			REQUIRE(rv[3] == v1[3] << v2[3]);
			REQUIRE(rv[4] == v1[4] << v2[4]);
			REQUIRE(rv[5] == v1[5] << v2[5]);
			REQUIRE(rv[6] == v1[6] << v2[6]);
			REQUIRE(rv[7] == v1[7] << v2[7]);
			// ==
			bool beq = (v1 == v2);
			REQUIRE(beq == (v1[0] == v2[0]));
			REQUIRE(beq == (v1[1] == v2[1]));
			REQUIRE(beq == (v1[2] == v2[2]));
			REQUIRE(beq == (v1[3] == v2[3]));
			REQUIRE(beq == (v1[4] == v2[4]));
			REQUIRE(beq == (v1[5] == v2[5]));
			REQUIRE(beq == (v1[6] == v2[6]));
			REQUIRE(beq == (v1[7] == v2[7]));
			// vals_equal
			auto eq = (vals_equal(v1, v2));
			REQUIRE(eq[0] == (v1[0] == v2[0]));
			REQUIRE(eq[1] == (v1[1] == v2[1]));
			REQUIRE(eq[2] == (v1[2] == v2[2]));
			REQUIRE(eq[3] == (v1[3] == v2[3]));
			REQUIRE(eq[4] == (v1[4] == v2[4]));
			REQUIRE(eq[5] == (v1[5] == v2[5]));
			REQUIRE(eq[6] == (v1[6] == v2[6]));
			REQUIRE(eq[7] == (v1[7] == v2[7]));
			// vals_greater
			auto gq = (vals_greater(v1, v2));
			REQUIRE(gq[0] == (v1[0] > v2[0]));
			REQUIRE(gq[1] == (v1[1] > v2[1]));
			REQUIRE(gq[2] == (v1[2] > v2[2]));
			REQUIRE(gq[3] == (v1[3] > v2[3]));
			REQUIRE(gq[4] == (v1[4] > v2[4]));
			REQUIRE(gq[5] == (v1[5] > v2[5]));
			REQUIRE(gq[6] == (v1[6] > v2[6]));
			REQUIRE(gq[7] == (v1[7] > v2[7]));
			// vals_greater_equal
			auto geq = (vals_greater_equal(v1, v2));
			REQUIRE(geq[0] == (v1[0] >= v2[0]));
			REQUIRE(geq[1] == (v1[1] >= v2[1]));
			REQUIRE(geq[2] == (v1[2] >= v2[2]));
			REQUIRE(geq[3] == (v1[3] >= v2[3]));
			REQUIRE(geq[4] == (v1[4] >= v2[4]));
			REQUIRE(geq[5] == (v1[5] >= v2[5]));
			REQUIRE(geq[6] == (v1[6] >= v2[6]));
			REQUIRE(geq[7] == (v1[7] >= v2[7]));
			// vals_smaller
			auto sq = (vals_smaller(v1, v2));
			REQUIRE(sq[0] == (v1[0] < v2[0]));
			REQUIRE(sq[1] == (v1[1] < v2[1]));
			REQUIRE(sq[2] == (v1[2] < v2[2]));
			REQUIRE(sq[3] == (v1[3] < v2[3]));
			REQUIRE(sq[4] == (v1[4] < v2[4]));
			REQUIRE(sq[5] == (v1[5] < v2[5]));
			REQUIRE(sq[6] == (v1[6] < v2[6]));
			REQUIRE(sq[7] == (v1[7] < v2[7]));
			// vals_smaller_equal
			auto seq = (vals_smaller_equal(v1, v2));
			REQUIRE(seq[0] == (v1[0] <= v2[0]));
			REQUIRE(seq[1] == (v1[1] <= v2[1]));
			REQUIRE(seq[2] == (v1[2] <= v2[2]));
			REQUIRE(seq[3] == (v1[3] <= v2[3]));
			REQUIRE(seq[4] == (v1[4] <= v2[4]));
			REQUIRE(seq[5] == (v1[5] <= v2[5]));
			REQUIRE(seq[6] == (v1[6] <= v2[6]));
			REQUIRE(seq[7] == (v1[7] <= v2[7]));
			// vals_not_equal
			auto neq = (vals_not_equal(v1, v2));
			REQUIRE(neq[0] == (v1[0] != v2[0]));
			REQUIRE(neq[1] == (v1[1] != v2[1]));
			REQUIRE(neq[2] == (v1[2] != v2[2]));
			REQUIRE(neq[3] == (v1[3] != v2[3]));
			REQUIRE(neq[4] == (v1[4] != v2[4]));
			REQUIRE(neq[5] == (v1[5] != v2[5]));
			REQUIRE(neq[6] == (v1[6] != v2[6]));
			REQUIRE(neq[7] == (v1[7] != v2[7]));
		}
	}
}