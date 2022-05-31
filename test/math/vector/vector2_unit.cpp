/*! @file   vector2_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Test unit for size 2 vectors.
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
		/// MATH: Col Vec2 Tests
		TEST_CASE("MathColInt32Vec2Test1")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			// Constructor
			const int32_t i1 = r(gen), i2 = r(gen);
			i32cvec2u_t v1 = { i1, i2 };
			REQUIRE(i1 == v1.x);
			REQUIRE(i2 == v1.y);
			// Assignment
			i32cvec2_t v2;
			v2 = v1;
			REQUIRE(i1 == v2.x);
			REQUIRE(i2 == v2.y);
		}
		TEST_CASE("MathColInt32Vec2Test2")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			i32cvec2u_t v1	 = { r(gen), r(gen) };
			i32cvec2_t v2		 = { r(gen), r(gen) };
			const int32_t rn = r(gen);

			// +
			auto rv = v1 + v2;
			REQUIRE(rv.x == v1.x + v2.x);
			REQUIRE(rv.y == v1.y + v2.y);
			// -
			rv = v1 - v2;
			REQUIRE(rv.x == v1.x - v2.x);
			REQUIRE(rv.y == v1.y - v2.y);
			// * (scalar)
			rv = v1 * rn;
			REQUIRE(rv.x == v1.x * rn);
			REQUIRE(rv.y == v1.y * rn);
			// / (scalar)
			rv = v1 / rn;
			REQUIRE(rv.x == v1.x / rn);
			REQUIRE(rv.y == v1.y / rn);
			// %
			rv = v1 % v2;
			REQUIRE(rv.x == v1.x % v2.x);
			REQUIRE(rv.y == v1.y % v2.y);
			// &
			rv = v1 & v2;
			REQUIRE(rv.x == (v1.x & v2.x));
			REQUIRE(rv.y == (v1.y & v2.y));
			// %
			rv = v1 | v2;
			REQUIRE(rv.x == (v1.x | v2.x));
			REQUIRE(rv.y == (v1.y | v2.y));
			// ^
			rv = v1 % v2;
			REQUIRE(rv.x == v1.x % v2.x);
			REQUIRE(rv.y == v1.y % v2.y);
			// ~
			rv = ~v1;
			REQUIRE(rv.x == ~v1.x);
			REQUIRE(rv.y == ~v1.y);
			// >>
			rv = v1 >> v2;
			REQUIRE(rv.x == v1.x >> v2.x);
			REQUIRE(rv.y == v1.y >> v2.y);
			// <<
			rv = v1 << v2;
			REQUIRE(rv.x == v1.x << v2.x);
			REQUIRE(rv.y == v1.y << v2.y);
			// ==
			bool beq = (v1 == v2);
			REQUIRE(beq == (v1.x == v2.x));
			REQUIRE(beq == (v1.y == v2.y));
			// vals_equal
			auto eq = (vals_equal(v1, v2));
			REQUIRE(eq.x == (v1.x == v2.x));
			REQUIRE(eq.y == (v1.y == v2.y));
			// vals_greater
			auto gq = (vals_greater(v1, v2));
			REQUIRE(gq.x == (v1.x > v2.x));
			REQUIRE(gq.y == (v1.y > v2.y));
			// vals_greater_equal
			auto geq = (vals_greater_equal(v1, v2));
			REQUIRE(geq.x == (v1.x >= v2.x));
			REQUIRE(geq.y == (v1.y >= v2.y));
			// vals_smaller
			auto sq = (vals_smaller(v1, v2));
			REQUIRE(sq.x == (v1.x < v2.x));
			REQUIRE(sq.y == (v1.y < v2.y));
			// vals_smaller_equal
			auto seq = (vals_smaller_equal(v1, v2));
			REQUIRE(seq.x == (v1.x <= v2.x));
			REQUIRE(seq.y == (v1.y <= v2.y));
			// vals_not_equal
			auto neq = (vals_not_equal(v1, v2));
			REQUIRE(neq.x == (v1.x != v2.x));
			REQUIRE(neq.y == (v1.y != v2.y));
		}

		/// MATH: Row Vec2 Tests
		TEST_CASE("MathRowInt32Vec2Test1")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			// Constructor
			const int32_t i1 = r(gen), i2 = r(gen);
			i32rvec2u_t v1 = { i1, i2 };
			REQUIRE(i1 == v1.x);
			REQUIRE(i2 == v1.y);
			// Assignment
			i32rvec2_t v2;
			v2 = v1;
			REQUIRE(i1 == v2.x);
			REQUIRE(i2 == v2.y);
		}
		TEST_CASE("MathRowInt32Vec2Test2")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			i32rvec2u_t v1	 = { r(gen), r(gen) };
			i32rvec2_t v2		 = { r(gen), r(gen) };
			const int32_t rn = r(gen);

			// +
			auto rv = v1 + v2;
			REQUIRE(rv.x == v1.x + v2.x);
			REQUIRE(rv.y == v1.y + v2.y);
			// -
			rv = v1 - v2;
			REQUIRE(rv.x == v1.x - v2.x);
			REQUIRE(rv.y == v1.y - v2.y);
			// * (scalar)
			rv = v1 * rn;
			REQUIRE(rv.x == v1.x * rn);
			REQUIRE(rv.y == v1.y * rn);
			// / (scalar)
			rv = v1 / rn;
			REQUIRE(rv.x == v1.x / rn);
			REQUIRE(rv.y == v1.y / rn);
			// %
			rv = v1 % v2;
			REQUIRE(rv.x == v1.x % v2.x);
			REQUIRE(rv.y == v1.y % v2.y);
			// &
			rv = v1 & v2;
			REQUIRE(rv.x == (v1.x & v2.x));
			REQUIRE(rv.y == (v1.y & v2.y));
			// %
			rv = v1 | v2;
			REQUIRE(rv.x == (v1.x | v2.x));
			REQUIRE(rv.y == (v1.y | v2.y));
			// ^
			rv = v1 % v2;
			REQUIRE(rv.x == v1.x % v2.x);
			REQUIRE(rv.y == v1.y % v2.y);
			// ~
			rv = ~v1;
			REQUIRE(rv.x == ~v1.x);
			REQUIRE(rv.y == ~v1.y);
			// >>
			rv = v1 >> v2;
			REQUIRE(rv.x == v1.x >> v2.x);
			REQUIRE(rv.y == v1.y >> v2.y);
			// <<
			rv = v1 << v2;
			REQUIRE(rv.x == v1.x << v2.x);
			REQUIRE(rv.y == v1.y << v2.y);
			// ==
			bool beq = (v1 == v2);
			REQUIRE(beq == (v1.x == v2.x));
			REQUIRE(beq == (v1.y == v2.y));
			// vals_equal
			auto eq = (vals_equal(v1, v2));
			REQUIRE(eq.x == (v1.x == v2.x));
			REQUIRE(eq.y == (v1.y == v2.y));
			// vals_greater
			auto gq = (vals_greater(v1, v2));
			REQUIRE(gq.x == (v1.x > v2.x));
			REQUIRE(gq.y == (v1.y > v2.y));
			// vals_greater_equal
			auto geq = (vals_greater_equal(v1, v2));
			REQUIRE(geq.x == (v1.x >= v2.x));
			REQUIRE(geq.y == (v1.y >= v2.y));
			// vals_smaller
			auto sq = (vals_smaller(v1, v2));
			REQUIRE(sq.x == (v1.x < v2.x));
			REQUIRE(sq.y == (v1.y < v2.y));
			// vals_smaller_equal
			auto seq = (vals_smaller_equal(v1, v2));
			REQUIRE(seq.x == (v1.x <= v2.x));
			REQUIRE(seq.y == (v1.y <= v2.y));
			// vals_not_equal
			auto neq = (vals_not_equal(v1, v2));
			REQUIRE(neq.x == (v1.x != v2.x));
			REQUIRE(neq.y == (v1.y != v2.y));
		}

		/// MATH: Vec2 Mul/Div Tests
		TEST_CASE("MathInt32Vec2MulDivTest")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			i32cvec2_t c1	 = { r(gen), r(gen) };
			i32rvec2u_t r1 = { r(gen), r(gen) };

			// *
			auto v1 = c1 * r1;
			REQUIRE(v1[0][0] == (c1.x * r1.x));
			REQUIRE(v1[0][1] == (c1.x * r1.y));
			REQUIRE(v1[1][0] == (c1.y * r1.x));
			REQUIRE(v1[1][1] == (c1.y * r1.y));
			auto v2 = r1 * c1;
			REQUIRE(v2 == (r1.x * c1.x + r1.y * c1.y));
			// /
			v1 = c1 / r1;
			REQUIRE(v1[0][0] == (c1.x / r1.x));
			REQUIRE(v1[0][1] == (c1.x / r1.y));
			REQUIRE(v1[1][0] == (c1.y / r1.x));
			REQUIRE(v1[1][1] == (c1.y / r1.y));
			v2 = r1 / c1;
			REQUIRE(v2 == (r1.x / c1.x + r1.y / c1.y));
		}
	}
}