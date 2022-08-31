/*! @file   vector3_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Test unit for size 3 vectors.
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
	/// MATH: Vec3 Tests
	TEST_CASE("MathInt32Vec3Test1")
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int32_t> r(
				std::numeric_limits<int32_t>::min(),
				std::numeric_limits<int32_t>::max());
		// Constructor
		const int32_t i1 = r(gen), i2 = r(gen), i3 = r(gen);
		i32vec3u_t v1 = { i1, i2, i3 };
		REQUIRE(i1 == v1.x);
		REQUIRE(i2 == v1.y);
		REQUIRE(i3 == v1.z);
		// Assignment
		i32vec3u_t v2;
		v2 = v1;
		REQUIRE(i1 == v2.x);
		REQUIRE(i2 == v2.y);
		REQUIRE(i3 == v2.z);
	}
	TEST_CASE("MathInt32Vec3Test2")
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int32_t> r(
				std::numeric_limits<int32_t>::min(),
				std::numeric_limits<int32_t>::max());
		i32vec3u_t v1		 = { r(gen), r(gen), r(gen) };
		i32vec3u_t v2		 = { r(gen), r(gen), r(gen) };
		const int32_t rn = r(gen);

		// +
		auto rv = v1 + v2;
		REQUIRE(rv.x == (v1.x + v2.x));
		REQUIRE(rv.y == (v1.y + v2.y));
		REQUIRE(rv.z == (v1.z + v2.z));
		// -
		rv = v1 - v2;
		REQUIRE(rv.x == (v1.x - v2.x));
		REQUIRE(rv.y == (v1.y - v2.y));
		REQUIRE(rv.z == (v1.z - v2.z));
		// * (scalar)
		rv = v1 * rn;
		REQUIRE(rv.x == (v1.x * rn));
		REQUIRE(rv.y == (v1.y * rn));
		REQUIRE(rv.z == (v1.z * rn));
		// / (scalar)
		rv = v1 / rn;
		REQUIRE(rv.x == (v1.x / rn));
		REQUIRE(rv.y == (v1.y / rn));
		REQUIRE(rv.z == (v1.z / rn));
		// %
		rv = v1 % v2;
		REQUIRE(rv.x == (v1.x % v2.x));
		REQUIRE(rv.y == (v1.y % v2.y));
		REQUIRE(rv.z == (v1.z % v2.z));
		// &
		rv = v1 & v2;
		REQUIRE(rv.x == (v1.x & v2.x));
		REQUIRE(rv.y == (v1.y & v2.y));
		REQUIRE(rv.z == (v1.z & v2.z));
		// %
		rv = v1 | v2;
		REQUIRE(rv.x == (v1.x | v2.x));
		REQUIRE(rv.y == (v1.y | v2.y));
		REQUIRE(rv.z == (v1.z | v2.z));
		// ^
		rv = v1 % v2;
		REQUIRE(rv.x == v1.x % v2.x);
		REQUIRE(rv.y == v1.y % v2.y);
		REQUIRE(rv.z == v1.z % v2.z);
		// ~
		rv = ~v1;
		REQUIRE(rv.x == ~v1.x);
		REQUIRE(rv.y == ~v1.y);
		REQUIRE(rv.z == ~v1.z);
		// >>
		rv = v1 >> v2;
		REQUIRE(rv.x == v1.x >> v2.x);
		REQUIRE(rv.y == v1.y >> v2.y);
		REQUIRE(rv.z == v1.z >> v2.z);
		// <<
		rv = v1 << v2;
		REQUIRE(rv.x == v1.x << v2.x);
		REQUIRE(rv.y == v1.y << v2.y);
		REQUIRE(rv.z == v1.z << v2.z);
		// ==
		bool beq = (v1 == v2);
		REQUIRE(beq == (v1.x == v2.x));
		REQUIRE(beq == (v1.y == v2.y));
		REQUIRE(beq == (v1.z == v2.z));
		// equal
		auto eq = (equal(v1, v2));
		REQUIRE(eq.x == (v1.x == v2.x));
		REQUIRE(eq.y == (v1.y == v2.y));
		REQUIRE(eq.z == (v1.z == v2.z));
		// not_equal
		auto neq = (not_equal(v1, v2));
		REQUIRE(neq.x == (v1.x != v2.x));
		REQUIRE(neq.y == (v1.y != v2.y));
		REQUIRE(neq.z == (v1.z != v2.z));
		// greater
		auto gq = (greater(v1, v2));
		REQUIRE(gq.x == (v1.x > v2.x));
		REQUIRE(gq.y == (v1.y > v2.y));
		REQUIRE(gq.z == (v1.z > v2.z));
		// greater_equal
		auto geq = (greater_equal(v1, v2));
		REQUIRE(geq.x == (v1.x >= v2.x));
		REQUIRE(geq.y == (v1.y >= v2.y));
		REQUIRE(geq.z == (v1.z >= v2.z));
		// smaller
		auto sq = (smaller(v1, v2));
		REQUIRE(sq.x == (v1.x < v2.x));
		REQUIRE(sq.y == (v1.y < v2.y));
		REQUIRE(sq.z == (v1.z < v2.z));
		// smaller_equal
		auto seq = (smaller_equal(v1, v2));
		REQUIRE(seq.x == (v1.x <= v2.x));
		REQUIRE(seq.y == (v1.y <= v2.y));
		REQUIRE(seq.z == (v1.z <= v2.z));
	}

	/// MATH: Vec3 Mul/Div Tests
	TEST_CASE("MathInt32Vec3MulDivTest")
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int32_t> r(
				std::numeric_limits<int32_t>::min(),
				std::numeric_limits<int32_t>::max());
		i32vec3u_t v1 = { r(gen), r(gen), r(gen) };
		i32vec3u_t u1 = { r(gen), r(gen), r(gen) };
		// *
		auto c1				= v1 * u1;
		REQUIRE(c1.x == v1.x * u1.x);
		REQUIRE(c1.y == v1.y * u1.y);
		REQUIRE(c1.z == v1.z * u1.z);
		// /
		c1 = v1 / u1;
		REQUIRE(c1.x == v1.x / u1.x);
		REQUIRE(c1.y == v1.y / u1.y);
		REQUIRE(c1.z == v1.z / u1.z);
	}
}