/*! @file   mathematics_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   16-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

// Include: Pulsar
#include "pulsar/math/linear_algebra.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Include: C++
#include <iostream>

// Pulsar
namespace pul
{
	/// MATHEMATICS: Matrix
	TEST_CASE("MathMatrixTest1")
	{
		math::vector4<float32_t> v1 = { 2.f, 3.f, 2.f, 9.f };
		math::vector2<float32_t> v2 = { 1.f, 0.f };
		math::vector4<float32_t> v3 = { 2.f, v2, 3.f };
		auto res										= v1 * math::transpose(v3);
	}
}