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
#include "pulsar/math/functions.hpp"

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
			i32mat2x2_t mat = {
				{{ r(gen), r(gen) },
					{ r(gen), r(gen) }}
			};
		}
	}
}