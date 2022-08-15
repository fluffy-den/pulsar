/*! @file   function_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   31-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

// Include: Pulsar
#include "pulsar/math/algorithm.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Pulsar
namespace pul
{
	// Math
	namespace math
	{
		/// MATH: Transpose Test
		TEST_CASE("RowVecTransposeTest")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			row_vector v1 = { r(gen), r(gen), r(gen), r(gen) };
			auto v2				= transpose(v1);
			REQUIRE(v1[0] == v2[0]);
			REQUIRE(v1[1] == v2[1]);
			REQUIRE(v1[2] == v2[2]);
			REQUIRE(v1[3] == v2[3]);
		}
		TEST_CASE("ColVecTransposeTest")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			col_vector v1 = { r(gen), r(gen), r(gen), r(gen) };
			auto v2				= transpose(v1);
			REQUIRE(v1[0] == v2[0]);
			REQUIRE(v1[1] == v2[1]);
			REQUIRE(v1[2] == v2[2]);
			REQUIRE(v1[3] == v2[3]);
		}
		TEST_CASE("MatTransposeTest")
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int32_t> r(
					std::numeric_limits<int32_t>::min(),
					std::numeric_limits<int32_t>::max());
			i32mat2x3u_t mat = {
				{{ r(gen), r(gen), r(gen) },
					{ r(gen), r(gen), r(gen) }}
			};
			auto trp = transpose(mat);
			REQUIRE(mat[0][0] == trp[0][0]);
			REQUIRE(mat[0][1] == trp[1][0]);
			REQUIRE(mat[0][2] == trp[2][0]);
			REQUIRE(mat[1][0] == trp[0][1]);
			REQUIRE(mat[1][1] == trp[1][1]);
			REQUIRE(mat[1][2] == trp[2][1]);
		}
		TEST_CASE("CastTest")
		{
			auto v1 = f32cvec4_t(1.0f, 1.0f, 2.0f, 0.2149f);
			auto v2 = cast<i32rvec4u_t>(v1);
			REQUIRE(v2.x == 1);
			REQUIRE(v2.y == 1);
			REQUIRE(v2.z == 2);
			REQUIRE(v2.w == 0);
			auto m1 = f32mat2_t{
				{{ 1.0f, 1.0f },
					{ 2.0f, 2.349f }}
			};
			auto m2 = cast<i32mat2u_t>(m1);
			REQUIRE(m2[0][0] == 1);
			REQUIRE(m2[0][1] == 1);
			REQUIRE(m2[1][0] == 2);
			REQUIRE(m2[1][1] == 2);
		}
	}
}