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
#include "pulsar/hash.hpp"
#include "pulsar/mathematics.hpp"
#include "pulsar/memory.hpp"
#include "pulsar/tuple.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Include: C++
#include <string>
#include <tuple>

// Pulsar
namespace pul
{
	/// TUPLE: Test Units
	TEST_CASE("TupleUnit1")
	{
		tuple t0 = { 0 };
		REQUIRE(i_get<0>(t0) == 0);
		tuple t1 = { 1, 2 };
		REQUIRE(i_get<0>(t1) == 1);
		REQUIRE(i_get<1>(t1) == 2);
		tuple t2 = { 6, 3, 1, 7u, 3u };
		REQUIRE(sizeof(t2) == 20);
		i_get<2>(t2) = 2;
		REQUIRE(i_get<2>(t2) == 2);
		tuple<int32_t, char, char, std::string, char, char> t3;
		REQUIRE(sizeof(t3) == sizeof(int32_t) + 4 * sizeof(char) + sizeof(std::string));
	}
	TEST_CASE("TupleUnit2")
	{
		fun_ptr fun = [](uint32_t i, uint64_t j) -> uint64_t
		{ return i + j; };
		tuple t1 = { 1u, 3ull };
		REQUIRE(sizeof(t1) == 2 * sizeof(uint64_t));
		REQUIRE(tuple_apply(fun, t1) == 4ull);
	}

	/// NUPLE: Test Units
	TEST_CASE("NupleUnit1")
	{
		nuple n0 = { nuval<$("nul_val")>(0) };
		REQUIRE(n_get<$("nul_val")>(n0) == 0);
		REQUIRE(i_get<0>(n0) == 0);
		nuple n1 = { nuval<$("one")>(1), nuval<$("two")>(2) };
		REQUIRE(n_get<$("one")>(n1) == 1);
		REQUIRE(i_get<0>(n1) == 1);
		REQUIRE(n_get<$("two")>(n1) == 2);
		REQUIRE(i_get<1>(n1) == 2);
		nuple n2 = {
			nuval<$("six")>(6),
			nuval<$("three")>(3),
			nuval<$("one")>(1),
			nuval<$("seven")>(7u),
			nuval<$("two")>(2u)
		};
		REQUIRE(sizeof(n2) == 20);
		n_get<$("seven")>(n2) = 77;
		REQUIRE(n_get<$("seven")>(n2) == 77);
		nuple<
				nutag<int32_t, $("v0")>,
				nutag<char, $("v1")>,
				nutag<char, $("v2")>,
				nutag<std::string, $("v3")>,
				nutag<char, $("v4")>,
				nutag<char, $("v5")>>
				n3;
		REQUIRE(n_offsetof<$("v3")>(n3) == 0);
		REQUIRE(n_offsetof<$("v4")>(n3) == i_offsetof<4>(n3));
		REQUIRE(n_sizeof<$("v3")>(n3) == sizeof(std::string));
		REQUIRE(i_sizeof<3>(n3) == n_sizeof<$("v3")>(n3));
		REQUIRE(sizeof(n3) == sizeof(int32_t) + 4 * sizeof(char) + sizeof(std::string));
	}
	TEST_CASE("NupleUnit2")
	{
		fun_ptr fun = [](uint32_t i, uint64_t j) -> uint64_t
		{
			return i + j;
		};
		nuple n1 = { nuval<$("i")>(1u), nuval<$("j")>(3ull) };
		REQUIRE(sizeof(n1) == 2 * sizeof(uint64_t));
		REQUIRE(tuple_apply(fun, n1) == 4ull);
	}
}