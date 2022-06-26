/*! @file   tuple_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   16-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

// Include: Pulsar
#include "pulsar/function.hpp"
#include "pulsar/tuple.hpp"


// Include: Catch2
#include "catch2/catch_all.hpp"

// Include: C++
#include <string>

// Pulsar
namespace pul
{
	/// TUPLE: Test Units
	TEST_CASE("TupleUnit1")
	{
		tuple t0 = { 0 };
		tuple t1 = { 1, 2 };
		REQUIRE(get<0>(t1) == 1);
		REQUIRE(get<1>(t1) == 2);
		tuple t2 = { 6, 3, 1, 7u, 3u };
		REQUIRE(sizeof(t2) == 20);
		get<2>(t2) = 2;
		REQUIRE(get<2>(t2) == 2);
		tuple<int32_t, char, char, std::string, char, char> t3;
		REQUIRE(sizeof(t3) == sizeof(int32_t) + 4 * sizeof(char) + sizeof(std::string));
		REQUIRE(alignof(t3) == 8);
	}
	TEST_CASE("TupleUnit2")
	{
		fun_ptr fun = [](uint32_t i, uint64_t j) -> uint64_t
		{ return i + j; };
		tuple t1 = { 1u, 3ull };
		REQUIRE(tuple_apply(fun, t1) == 4ull);
	}
}