/*! @file   fun_buf_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief	Test function buffers.
 *  @date   16-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

// Include: Pulsar
#include "pulsar/function.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Pulsar
namespace pul
{
	// Function Buffer Tests
	TEST_CASE("FunctionBufferTest1")
	{
		int64_t j		= 2;
		auto lambda = [&](int64_t __i)
		{ return __i + j + 1; };
		fun_buf p = lambda;
		auto task = [](void *) -> void
		{ return; };
		fun_buf t = task;
		t(nullptr);
		REQUIRE(p(1) == 4);
	}
}