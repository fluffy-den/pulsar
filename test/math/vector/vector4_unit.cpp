/*! @file   vector4_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Test unit for size 4 vectors.
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
		/// MATH: Col Vec4 Tests
		TEST_CASE("MathColInt32Vec4Test1")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			// Constructor
			const int32_t i1 = r(gen), i2 = r(gen), i3 = r(gen), i4 = r(gen);
			i32cvec4u_t v1 = { i1, i2, i3, i4 };
			REQUIRE(i1 == v1.x);
			REQUIRE(i2 == v1.y);
			REQUIRE(i3 == v1.z);
			REQUIRE(i4 == v1.w);
			// Assignment
			i32cvec4_t v2;
			v2 = v1;
			REQUIRE(i1 == v2.x);
			REQUIRE(i2 == v2.y);
			REQUIRE(i3 == v2.z);
			REQUIRE(i4 == v2.w);
		}
		TEST_CASE("MathColInt32Vec4Test2")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			i32cvec4u_t v1	 = { r(gen), r(gen), r(gen), r(gen) };
			i32cvec4_t v2		 = { r(gen), r(gen), r(gen), r(gen) };
			const int32_t rn = r(gen);

			// +
			auto rv = v1 + v2;
			REQUIRE(rv.x == (v1.x + v2.x));
			REQUIRE(rv.y == (v1.y + v2.y));
			REQUIRE(rv.z == (v1.z + v2.z));
			REQUIRE(rv.w == (v1.w + v2.w));
			// -
			rv = v1 - v2;
			REQUIRE(rv.x == (v1.x - v2.x));
			REQUIRE(rv.y == (v1.y - v2.y));
			REQUIRE(rv.z == (v1.z - v2.z));
			REQUIRE(rv.w == (v1.w - v2.w));
			// * (scalar)
			rv = v1 * rn;
			REQUIRE(rv.x == (v1.x * rn));
			REQUIRE(rv.y == (v1.y * rn));
			REQUIRE(rv.z == (v1.z * rn));
			REQUIRE(rv.w == (v1.w * rn));
			// / (scalar)
			rv = v1 / rn;
			REQUIRE(rv.x == (v1.x / rn));
			REQUIRE(rv.y == (v1.y / rn));
			REQUIRE(rv.z == (v1.z / rn));
			REQUIRE(rv.w == (v1.w / rn));
			// %
			rv = v1 % v2;
			REQUIRE(rv.x == (v1.x % v2.x));
			REQUIRE(rv.y == (v1.y % v2.y));
			REQUIRE(rv.z == (v1.z % v2.z));
			REQUIRE(rv.w == (v1.w % v2.w));
			// &
			rv = v1 & v2;
			REQUIRE(rv.x == (v1.x & v2.x));
			REQUIRE(rv.y == (v1.y & v2.y));
			REQUIRE(rv.z == (v1.z & v2.z));
			REQUIRE(rv.w == (v1.w & v2.w));
			// %
			rv = v1 | v2;
			REQUIRE(rv.x == (v1.x | v2.x));
			REQUIRE(rv.y == (v1.y | v2.y));
			REQUIRE(rv.z == (v1.z | v2.z));
			REQUIRE(rv.w == (v1.w | v2.w));
			// ^
			rv = v1 % v2;
			REQUIRE(rv.x == v1.x % v2.x);
			REQUIRE(rv.y == v1.y % v2.y);
			REQUIRE(rv.z == v1.z % v2.z);
			REQUIRE(rv.w == v1.w % v2.w);
			// ~
			rv = ~v1;
			REQUIRE(rv.x == ~v1.x);
			REQUIRE(rv.y == ~v1.y);
			REQUIRE(rv.z == ~v1.z);
			REQUIRE(rv.w == ~v1.w);
			// >>
			rv = v1 >> v2;
			REQUIRE(rv.x == v1.x >> v2.x);
			REQUIRE(rv.y == v1.y >> v2.y);
			REQUIRE(rv.z == v1.z >> v2.z);
			REQUIRE(rv.w == v1.w >> v2.w);
			// <<
			rv = v1 << v2;
			REQUIRE(rv.x == v1.x << v2.x);
			REQUIRE(rv.y == v1.y << v2.y);
			REQUIRE(rv.z == v1.z << v2.z);
			REQUIRE(rv.w == v1.w << v2.w);
			// ==
			bool beq = (v1 == v2);
			REQUIRE(beq == (v1.x == v2.x));
			REQUIRE(beq == (v1.y == v2.y));
			REQUIRE(beq == (v1.z == v2.z));
			REQUIRE(beq == (v1.w == v2.w));
			// vals_equal
			auto eq = (vals_equal(v1, v2));
			REQUIRE(eq.x == (v1.x == v2.x));
			REQUIRE(eq.y == (v1.y == v2.y));
			REQUIRE(eq.z == (v1.z == v2.z));
			REQUIRE(eq.w == (v1.w == v2.w));
			// vals_greater
			auto gq = (vals_greater(v1, v2));
			REQUIRE(gq.x == (v1.x > v2.x));
			REQUIRE(gq.y == (v1.y > v2.y));
			REQUIRE(gq.z == (v1.z > v2.z));
			REQUIRE(gq.w == (v1.w > v2.w));
			// vals_greater_equal
			auto geq = (vals_greater_equal(v1, v2));
			REQUIRE(geq.x == (v1.x >= v2.x));
			REQUIRE(geq.y == (v1.y >= v2.y));
			REQUIRE(geq.z == (v1.z >= v2.z));
			REQUIRE(geq.w == (v1.w >= v2.w));
			// vals_smaller
			auto sq = (vals_smaller(v1, v2));
			REQUIRE(sq.x == (v1.x < v2.x));
			REQUIRE(sq.y == (v1.y < v2.y));
			REQUIRE(sq.z == (v1.z < v2.z));
			REQUIRE(sq.w == (v1.w < v2.w));
			// vals_smaller_equal
			auto seq = (vals_smaller_equal(v1, v2));
			REQUIRE(seq.x == (v1.x <= v2.x));
			REQUIRE(seq.y == (v1.y <= v2.y));
			REQUIRE(seq.z == (v1.z <= v2.z));
			REQUIRE(seq.w == (v1.w <= v2.w));
			// vals_not_equal
			auto neq = (vals_not_equal(v1, v2));
			REQUIRE(neq.x == (v1.x != v2.x));
			REQUIRE(neq.y == (v1.y != v2.y));
			REQUIRE(neq.z == (v1.z != v2.z));
			REQUIRE(neq.w == (v1.w != v2.w));
		}

		/// MATH: Row Vec4 Tests
		TEST_CASE("MathRowInt32Vec4Test1")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			// Constructor
			const int32_t i1 = r(gen), i2 = r(gen), i3 = r(gen), i4 = r(gen);
			i32rvec4u_t v1 = { i1, i2, i3, i4 };
			REQUIRE(i1 == v1.x);
			REQUIRE(i2 == v1.y);
			REQUIRE(i3 == v1.z);
			REQUIRE(i4 == v1.w);
			// Assignment
			i32rvec4_t v2;
			v2 = v1;
			REQUIRE(i1 == v2.x);
			REQUIRE(i2 == v2.y);
			REQUIRE(i3 == v2.z);
			REQUIRE(i4 == v2.w);
		}
		TEST_CASE("MathRowInt32Vec4Test2")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			i32rvec4u_t v1	 = { r(gen), r(gen), r(gen), r(gen) };
			i32rvec4_t v2		 = { r(gen), r(gen), r(gen), r(gen) };
			const int32_t rn = r(gen);

			// +
			auto rv = v1 + v2;
			REQUIRE(rv.x == (v1.x + v2.x));
			REQUIRE(rv.y == (v1.y + v2.y));
			REQUIRE(rv.z == (v1.z + v2.z));
			REQUIRE(rv.w == (v1.w + v2.w));
			// -
			rv = v1 - v2;
			REQUIRE(rv.x == (v1.x - v2.x));
			REQUIRE(rv.y == (v1.y - v2.y));
			REQUIRE(rv.z == (v1.z - v2.z));
			REQUIRE(rv.w == (v1.w - v2.w));
			// * (scalar)
			rv = v1 * rn;
			REQUIRE(rv.x == (v1.x * rn));
			REQUIRE(rv.y == (v1.y * rn));
			REQUIRE(rv.z == (v1.z * rn));
			REQUIRE(rv.w == (v1.w * rn));
			// / (scalar)
			rv = v1 / rn;
			REQUIRE(rv.x == (v1.x / rn));
			REQUIRE(rv.y == (v1.y / rn));
			REQUIRE(rv.z == (v1.z / rn));
			REQUIRE(rv.w == (v1.w / rn));
			// %
			rv = v1 % v2;
			REQUIRE(rv.x == (v1.x % v2.x));
			REQUIRE(rv.y == (v1.y % v2.y));
			REQUIRE(rv.z == (v1.z % v2.z));
			REQUIRE(rv.w == (v1.w % v2.w));
			// &
			rv = v1 & v2;
			REQUIRE(rv.x == (v1.x & v2.x));
			REQUIRE(rv.y == (v1.y & v2.y));
			REQUIRE(rv.z == (v1.z & v2.z));
			REQUIRE(rv.w == (v1.w & v2.w));
			// %
			rv = v1 | v2;
			REQUIRE(rv.x == (v1.x | v2.x));
			REQUIRE(rv.y == (v1.y | v2.y));
			REQUIRE(rv.z == (v1.z | v2.z));
			REQUIRE(rv.w == (v1.w | v2.w));
			// ^
			rv = v1 % v2;
			REQUIRE(rv.x == v1.x % v2.x);
			REQUIRE(rv.y == v1.y % v2.y);
			REQUIRE(rv.z == v1.z % v2.z);
			REQUIRE(rv.w == v1.w % v2.w);
			// ~
			rv = ~v1;
			REQUIRE(rv.x == ~v1.x);
			REQUIRE(rv.y == ~v1.y);
			REQUIRE(rv.z == ~v1.z);
			REQUIRE(rv.w == ~v1.w);
			// >>
			rv = v1 >> v2;
			REQUIRE(rv.x == v1.x >> v2.x);
			REQUIRE(rv.y == v1.y >> v2.y);
			REQUIRE(rv.z == v1.z >> v2.z);
			REQUIRE(rv.w == v1.w >> v2.w);
			// <<
			rv = v1 << v2;
			REQUIRE(rv.x == v1.x << v2.x);
			REQUIRE(rv.y == v1.y << v2.y);
			REQUIRE(rv.z == v1.z << v2.z);
			REQUIRE(rv.w == v1.w << v2.w);
			// ==
			bool beq = (v1 == v2);
			REQUIRE(beq == (v1.x == v2.x));
			REQUIRE(beq == (v1.y == v2.y));
			REQUIRE(beq == (v1.z == v2.z));
			REQUIRE(beq == (v1.w == v2.w));
			// vals_equal
			auto eq = (vals_equal(v1, v2));
			REQUIRE(eq.x == (v1.x == v2.x));
			REQUIRE(eq.y == (v1.y == v2.y));
			REQUIRE(eq.z == (v1.z == v2.z));
			REQUIRE(eq.w == (v1.w == v2.w));
			// vals_greater
			auto gq = (vals_greater(v1, v2));
			REQUIRE(gq.x == (v1.x > v2.x));
			REQUIRE(gq.y == (v1.y > v2.y));
			REQUIRE(gq.z == (v1.z > v2.z));
			REQUIRE(gq.w == (v1.w > v2.w));
			// vals_greater_equal
			auto geq = (vals_greater_equal(v1, v2));
			REQUIRE(geq.x == (v1.x >= v2.x));
			REQUIRE(geq.y == (v1.y >= v2.y));
			REQUIRE(geq.z == (v1.z >= v2.z));
			REQUIRE(geq.w == (v1.w >= v2.w));
			// vals_smaller
			auto sq = (vals_smaller(v1, v2));
			REQUIRE(sq.x == (v1.x < v2.x));
			REQUIRE(sq.y == (v1.y < v2.y));
			REQUIRE(sq.z == (v1.z < v2.z));
			REQUIRE(sq.w == (v1.w < v2.w));
			// vals_smaller_equal
			auto seq = (vals_smaller_equal(v1, v2));
			REQUIRE(seq.x == (v1.x <= v2.x));
			REQUIRE(seq.y == (v1.y <= v2.y));
			REQUIRE(seq.z == (v1.z <= v2.z));
			REQUIRE(seq.w == (v1.w <= v2.w));
			// vals_not_equal
			auto neq = (vals_not_equal(v1, v2));
			REQUIRE(neq.x == (v1.x != v2.x));
			REQUIRE(neq.y == (v1.y != v2.y));
			REQUIRE(neq.z == (v1.z != v2.z));
			REQUIRE(neq.w == (v1.w != v2.w));
		}

		/// MATH: Vec4 Mul/Div Tests
		TEST_CASE("MathInt32Vec2MulDivTest")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			i32cvec4_t c1	 = { r(gen), r(gen), r(gen), r(gen) };
			i32rvec4u_t r1 = { r(gen), r(gen), r(gen), r(gen) };

			// *
			auto v1 = c1 * r1;
			REQUIRE(v1[0][0] == (c1.x * r1.x));
			REQUIRE(v1[0][1] == (c1.x * r1.y));
			REQUIRE(v1[0][2] == (c1.x * r1.z));
			REQUIRE(v1[0][3] == (c1.x * r1.w));
			REQUIRE(v1[1][0] == (c1.y * r1.x));
			REQUIRE(v1[1][1] == (c1.y * r1.y));
			REQUIRE(v1[1][2] == (c1.y * r1.z));
			REQUIRE(v1[1][3] == (c1.y * r1.w));
			REQUIRE(v1[2][0] == (c1.z * r1.x));
			REQUIRE(v1[2][1] == (c1.z * r1.y));
			REQUIRE(v1[2][2] == (c1.z * r1.z));
			REQUIRE(v1[2][3] == (c1.z * r1.w));
			REQUIRE(v1[3][0] == (c1.w * r1.x));
			REQUIRE(v1[3][1] == (c1.w * r1.y));
			REQUIRE(v1[3][2] == (c1.w * r1.z));
			REQUIRE(v1[3][3] == (c1.w * r1.w));
			auto v2 = r1 * c1;
			REQUIRE(v2 == (r1.x * c1.x + r1.y * c1.y + r1.z * c1.z + r1.w * c1.w));
			// /
			v1 = c1 / r1;
			REQUIRE(v1[0][0] == (c1.x / r1.x));
			REQUIRE(v1[0][1] == (c1.x / r1.y));
			REQUIRE(v1[0][2] == (c1.x / r1.z));
			REQUIRE(v1[0][3] == (c1.x / r1.w));
			REQUIRE(v1[1][0] == (c1.y / r1.x));
			REQUIRE(v1[1][1] == (c1.y / r1.y));
			REQUIRE(v1[1][2] == (c1.y / r1.z));
			REQUIRE(v1[1][3] == (c1.y / r1.w));
			REQUIRE(v1[2][0] == (c1.z / r1.x));
			REQUIRE(v1[2][1] == (c1.z / r1.y));
			REQUIRE(v1[2][2] == (c1.z / r1.z));
			REQUIRE(v1[2][3] == (c1.z / r1.w));
			REQUIRE(v1[3][0] == (c1.w / r1.x));
			REQUIRE(v1[3][1] == (c1.w / r1.y));
			REQUIRE(v1[3][2] == (c1.w / r1.z));
			REQUIRE(v1[3][3] == (c1.w / r1.w));
			v2 = r1 / c1;
			REQUIRE(v2 == (r1.x / c1.x + r1.y / c1.y + r1.z / c1.z + r1.w / c1.w));
		}
	}
}