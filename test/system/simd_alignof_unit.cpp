/*! @file   intrin_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   24-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

// Include: Pulsar
#include "pulsar/intrin.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Pulsar
namespace pul
{
	// SIMD Alignment
	TEST_CASE("SIMDAlignmentTestInt8")
	{
		REQUIRE(simd_alignment_of_v<int8_t, 1, SIMD_ALIGNED> == alignof(int8_t));
		REQUIRE(simd_alignment_of_v<int8_t, 2, SIMD_ALIGNED> == alignof(int8_t));
		REQUIRE(simd_alignment_of_v<int8_t, 3, SIMD_ALIGNED> == alignof(int8_t));
		REQUIRE(simd_alignment_of_v<int8_t, 4, SIMD_ALIGNED> == alignof(int8_t));
	}
	TEST_CASE("SIMDAlignmentTestInt16")
	{
		REQUIRE(simd_alignment_of_v<int16_t, 1, SIMD_ALIGNED> == alignof(int16_t));
		REQUIRE(simd_alignment_of_v<int16_t, 2, SIMD_ALIGNED> == alignof(int16_t));
		REQUIRE(simd_alignment_of_v<int16_t, 3, SIMD_ALIGNED> == alignof(int16_t));
		REQUIRE(simd_alignment_of_v<int16_t, 4, SIMD_ALIGNED> == 8);
	}
	TEST_CASE("SIMDAlignmentTestInt32")
	{
		REQUIRE(simd_alignment_of_v<int32_t, 1, SIMD_ALIGNED> == alignof(int32_t));
		REQUIRE(simd_alignment_of_v<int32_t, 2, SIMD_ALIGNED> == 8);
		REQUIRE(simd_alignment_of_v<int32_t, 3, SIMD_ALIGNED> == alignof(int32_t));
		REQUIRE(simd_alignment_of_v<int32_t, 4, SIMD_ALIGNED> == 16);
	}
	TEST_CASE("SIMDAlignmentTestInt64")
	{
		REQUIRE(simd_alignment_of_v<int64_t, 1, SIMD_ALIGNED> == alignof(int64_t));
		REQUIRE(simd_alignment_of_v<int64_t, 2, SIMD_ALIGNED> == 16);
		REQUIRE(simd_alignment_of_v<int64_t, 3, SIMD_ALIGNED> == alignof(int64_t));
		REQUIRE(simd_alignment_of_v<int64_t, 4, SIMD_ALIGNED> == 32);
	}
}