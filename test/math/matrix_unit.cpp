/*! @file   matrix_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Test unit for matrices.
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
#include <type_traits>


// Pulsar
namespace pul
{
	// Math
	namespace math
	{
		/// MATH: Matrix Tests
		TEST_CASE("MathMatrixTest1")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			// Constructor
			int32_t i1 = r(gen), i2 = r(gen), i3 = r(gen), i4 = r(gen);
			i32mat2_t m1 = {
				{{ i1, i2 },
					{ i3, i4 }}
			};
			REQUIRE(i1 == m1[0][0]);
			REQUIRE(i2 == m1[0][1]);
			REQUIRE(i3 == m1[1][0]);
			REQUIRE(i4 == m1[1][1]);
			// Assignment
			i32mat2_t m2;
			m2 = m1;
			REQUIRE(i1 == m2[0][0]);
			REQUIRE(i2 == m2[0][1]);
			REQUIRE(i3 == m2[1][0]);
			REQUIRE(i4 == m2[1][1]);
		}
		TEST_CASE("MathMatrixTest2")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			i32mat2_t m1 = {
				{{ r(gen), r(gen) },
					{ r(gen), r(gen) }}
			};
			i32mat2_t m2 = {
				{{ r(gen), r(gen) },
					{ r(gen), r(gen) }}
			};
			const int32_t rn = r(gen);

			// +
			auto rv = m1 + m2;
			REQUIRE(rv[0][0] == (m1[0][0] + m2[0][0]));
			REQUIRE(rv[0][1] == (m1[0][1] + m2[0][1]));
			REQUIRE(rv[1][0] == (m1[1][0] + m2[1][0]));
			REQUIRE(rv[1][1] == (m1[1][1] + m2[1][1]));
			// -
			rv = m1 - m2;
			REQUIRE(rv[0][0] == (m1[0][0] - m2[0][0]));
			REQUIRE(rv[0][1] == (m1[0][1] - m2[0][1]));
			REQUIRE(rv[1][0] == (m1[1][0] - m2[1][0]));
			REQUIRE(rv[1][1] == (m1[1][1] - m2[1][1]));
			// * (scalar)
			rv = m1 * rn;
			REQUIRE(rv[0][0] == (m1[0][0] * rn));
			REQUIRE(rv[0][1] == (m1[0][1] * rn));
			REQUIRE(rv[1][0] == (m1[1][0] * rn));
			REQUIRE(rv[1][1] == (m1[1][1] * rn));
			// / (scalar)
			rv = m1 / rn;
			REQUIRE(rv[0][0] == (m1[0][0] / rn));
			REQUIRE(rv[0][1] == (m1[0][1] / rn));
			REQUIRE(rv[1][0] == (m1[1][0] / rn));
			REQUIRE(rv[1][1] == (m1[1][1] / rn));
			// %
			rv = m1 % m2;
			REQUIRE(rv[0][0] == (m1[0][0] % m2[0][0]));
			REQUIRE(rv[0][1] == (m1[0][1] % m2[0][1]));
			REQUIRE(rv[1][0] == (m1[1][0] % m2[1][0]));
			REQUIRE(rv[1][1] == (m1[1][1] % m2[1][1]));
			// &
			rv = m1 & m2;
			REQUIRE(rv[0][0] == (m1[0][0] & m2[0][0]));
			REQUIRE(rv[0][1] == (m1[0][1] & m2[0][1]));
			REQUIRE(rv[1][0] == (m1[1][0] & m2[1][0]));
			REQUIRE(rv[1][1] == (m1[1][1] & m2[1][1]));
			// %
			rv = m1 | m2;
			REQUIRE(rv[0][0] == (m1[0][0] | m2[0][0]));
			REQUIRE(rv[0][1] == (m1[0][1] | m2[0][1]));
			REQUIRE(rv[1][0] == (m1[1][0] | m2[1][0]));
			REQUIRE(rv[1][1] == (m1[1][1] | m2[1][1]));
			// ^
			rv = m1 % m2;
			REQUIRE(rv[0][0] == m1[0][0] % m2[0][0]);
			REQUIRE(rv[0][1] == m1[0][1] % m2[0][1]);
			REQUIRE(rv[1][0] == m1[1][0] % m2[1][0]);
			REQUIRE(rv[1][1] == m1[1][1] % m2[1][1]);
			// ~
			rv = ~m1;
			REQUIRE(rv[0][0] == ~m1[0][0]);
			REQUIRE(rv[0][1] == ~m1[0][1]);
			REQUIRE(rv[1][0] == ~m1[1][0]);
			REQUIRE(rv[1][1] == ~m1[1][1]);
			// >>
			rv = m1 >> m2;
			REQUIRE(rv[0][0] == m1[0][0] >> m2[0][0]);
			REQUIRE(rv[0][1] == m1[0][1] >> m2[0][1]);
			REQUIRE(rv[1][0] == m1[1][0] >> m2[1][0]);
			REQUIRE(rv[1][1] == m1[1][1] >> m2[1][1]);
			// <<
			rv = m1 << m2;
			REQUIRE(rv[0][0] == m1[0][0] << m2[0][0]);
			REQUIRE(rv[0][1] == m1[0][1] << m2[0][1]);
			REQUIRE(rv[1][0] == m1[1][0] << m2[1][0]);
			REQUIRE(rv[1][1] == m1[1][1] << m2[1][1]);
			// ==
			bool beq = (m1 == m2);
			REQUIRE(beq == (m1[0][0] == m2[0][0]));
			REQUIRE(beq == (m1[0][1] == m2[0][1]));
			REQUIRE(beq == (m1[1][0] == m2[1][0]));
			REQUIRE(beq == (m1[1][1] == m2[1][1]));
			// vals_equal
			auto eq = (vals_equal(m1, m2));
			REQUIRE(eq[0][0] == (m1[0][0] == m2[0][0]));
			REQUIRE(eq[0][1] == (m1[0][1] == m2[0][1]));
			REQUIRE(eq[1][0] == (m1[1][0] == m2[1][0]));
			REQUIRE(eq[1][1] == (m1[1][1] == m2[1][1]));
			// vals_greater
			auto gq = (vals_greater(m1, m2));
			REQUIRE(gq[0][0] == (m1[0][0] > m2[0][0]));
			REQUIRE(gq[0][1] == (m1[0][1] > m2[0][1]));
			REQUIRE(gq[1][0] == (m1[1][0] > m2[1][0]));
			REQUIRE(gq[1][1] == (m1[1][1] > m2[1][1]));
			// vals_greater_equal
			auto geq = (vals_greater_equal(m1, m2));
			REQUIRE(geq[0][0] == (m1[0][0] >= m2[0][0]));
			REQUIRE(geq[0][1] == (m1[0][1] >= m2[0][1]));
			REQUIRE(geq[1][0] == (m1[1][0] >= m2[1][0]));
			REQUIRE(geq[1][1] == (m1[1][1] >= m2[1][1]));
			// vals_smaller
			auto sq = (vals_smaller(m1, m2));
			REQUIRE(sq[0][0] == (m1[0][0] < m2[0][0]));
			REQUIRE(sq[0][1] == (m1[0][1] < m2[0][1]));
			REQUIRE(sq[1][0] == (m1[1][0] < m2[1][0]));
			REQUIRE(sq[1][1] == (m1[1][1] < m2[1][1]));
			// vals_smaller_equal
			auto seq = (vals_smaller_equal(m1, m2));
			REQUIRE(seq[0][0] == (m1[0][0] <= m2[0][0]));
			REQUIRE(seq[0][1] == (m1[0][1] <= m2[0][1]));
			REQUIRE(seq[1][0] == (m1[1][0] <= m2[1][0]));
			REQUIRE(seq[1][1] == (m1[1][1] <= m2[1][1]));
			// vals_not_equal
			auto neq = (vals_not_equal(m1, m2));
			REQUIRE(neq[0][0] == (m1[0][0] != m2[0][0]));
			REQUIRE(neq[0][1] == (m1[0][1] != m2[0][1]));
			REQUIRE(neq[1][0] == (m1[1][0] != m2[1][0]));
			REQUIRE(neq[1][1] == (m1[1][1] != m2[1][1]));
			// data
			auto dat = m1.data();
			REQUIRE(dat != nullptr);
		}

		/// MATH: Mat4 Mul/Div Tests
		TEST_CASE("MathInt32Mat2MulDivTest")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			i32mat2_t m1 = {
				{{ r(gen), r(gen) },
					{ r(gen), r(gen) }}
			};
			i32mat2u_t m2 = {
				{{ r(gen), r(gen) },
					{ r(gen), r(gen) }}
			};

			// mat2x2 * mat2x2 mul/div test
			// *
			auto r1 = m1 * m2;
			REQUIRE(r1[0][0] == (m1[0][0] * m2[0][0] + m1[0][1] * m2[1][0]));
			REQUIRE(r1[0][1] == (m1[0][0] * m2[0][1] + m1[0][1] * m2[1][1]));
			REQUIRE(r1[1][0] == (m1[1][0] * m2[0][0] + m1[1][1] * m2[1][0]));
			REQUIRE(r1[1][1] == (m1[1][0] * m2[0][1] + m1[1][1] * m2[1][1]));
			r1 = m2 * m1;
			REQUIRE(r1[0][0] == (m2[0][0] * m1[0][0] + m2[0][1] * m1[1][0]));
			REQUIRE(r1[0][1] == (m2[0][0] * m1[0][1] + m2[0][1] * m1[1][1]));
			REQUIRE(r1[1][0] == (m2[1][0] * m1[0][0] + m2[1][1] * m1[1][0]));
			REQUIRE(r1[1][1] == (m2[1][0] * m1[0][1] + m2[1][1] * m1[1][1]));
			// /
			r1 = m1 / m2;
			REQUIRE(r1[0][0] == (m1[0][0] / m2[0][0] + m1[0][1] / m2[1][0]));
			REQUIRE(r1[0][1] == (m1[0][0] / m2[0][1] + m1[0][1] / m2[1][1]));
			REQUIRE(r1[1][0] == (m1[1][0] / m2[0][0] + m1[1][1] / m2[1][0]));
			REQUIRE(r1[1][1] == (m1[1][0] / m2[0][1] + m1[1][1] / m2[1][1]));
			r1 = m2 / m1;
			REQUIRE(r1[0][0] == (m2[0][0] / m1[0][0] + m2[0][1] / m1[1][0]));
			REQUIRE(r1[0][1] == (m2[0][0] / m1[0][1] + m2[0][1] / m1[1][1]));
			REQUIRE(r1[1][0] == (m2[1][0] / m1[0][0] + m2[1][1] / m1[1][0]));
			REQUIRE(r1[1][1] == (m2[1][0] / m1[0][1] + m2[1][1] / m1[1][1]));

			// mat2x2 * cvec2 mul/div test
			i32cvec2u_t v1 = { r(gen), r(gen) };

			// *
			auto r2 = m1 * v1;
			REQUIRE(r2[0] == (m1[0][0] * v1[0] + m1[0][1] * v1[1]));
			REQUIRE(r2[1] == (m1[1][0] * v1[0] + m1[1][1] * v1[1]));
			// /
			r2 = m1 / v1;
			REQUIRE(r2[0] == (m1[0][0] / v1[0] + m1[0][1] / v1[1]));
			REQUIRE(r2[1] == (m1[1][0] / v1[0] + m1[1][1] / v1[1]));

			// rvec2 * cvec2
			i32rvec2_t v2 = { r(gen), r(gen) };

			// *
			auto r3 = v2 * v1;
			REQUIRE(r3 == (v2[0] * v1[0] + v2[1] * v1[1]));
			// /
			r3 = v2 / v1;
			REQUIRE(r3 == (v2[0] / v1[0] + v2[1] / v1[1]));

			// cvec2 * rvec3
			i32rvec3u_t v3 = { r(gen), r(gen), r(gen) };

			// *
			auto r4 = v1 * v3;
			REQUIRE(r4[0][0] == v1[0] * v3[0]);
			REQUIRE(r4[0][1] == v1[0] * v3[1]);
			REQUIRE(r4[0][2] == v1[0] * v3[2]);
			REQUIRE(r4[1][0] == v1[1] * v3[0]);
			REQUIRE(r4[1][1] == v1[1] * v3[1]);
			REQUIRE(r4[1][2] == v1[1] * v3[2]);
			// /
			r4 = v1 / v3;
			REQUIRE(r4[0][0] == v1[0] / v3[0]);
			REQUIRE(r4[0][1] == v1[0] / v3[1]);
			REQUIRE(r4[0][2] == v1[0] / v3[2]);
			REQUIRE(r4[1][0] == v1[1] / v3[0]);
			REQUIRE(r4[1][1] == v1[1] / v3[1]);
			REQUIRE(r4[1][2] == v1[1] / v3[2]);

			// rvec2 * mat2x2
			// *
			auto r5 = v2 * m1;
			REQUIRE(r5[0] == v2[0] * m1[0][0] + v2[1] * m1[0][1]);
			REQUIRE(r5[1] == v2[0] * m1[1][0] + v2[1] * m1[1][1]);
			// /
			r5 = v2 / m1;
			REQUIRE(r5[0] == v2[0] / m1[0][0] + v2[1] / m1[0][1]);
			REQUIRE(r5[1] == v2[0] / m1[1][0] + v2[1] / m1[1][1]);
		}
	}
}