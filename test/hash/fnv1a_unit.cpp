/*! @file   fnv1a_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   01-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

// Include: Pulsar
#include "pulsar/hash.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Include: Random
#include <random>

// Pulsar
namespace pul
{
	/// FNV1-A: Test Unit
	TEST_CASE("Fnv1aUnit1")
	{
		const char str[] = "Blah!";
		const char *ptr	 = &str[0];

		size_t val64 = fnv1a::hash64(ptr, sizeof(str));
		REQUIRE(val64 != 0);
		size_t val32 = fnv1a::hash32(ptr, sizeof(str));
		REQUIRE(val32 != 0);
#ifdef PF_64BIT
		REQUIRE(fnv1a::hash(str) == val64);
#else	 // ^^^ PF_64BIT ^^^ / vvv PF_64BIT vvv
		REQUIRE(fnv1a::hash(str) == val32);
#endif // PF_64BIT
	}
	TEST_CASE("Fnv1aUnit2")
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int32_t> r(
				std::numeric_limits<int32_t>::min(),
				std::numeric_limits<int32_t>::max());
		int32_t rdv = r(gen);

		hash_fnv1a<int32_t> hasher;
		size_t hvl = hasher(rdv);
		REQUIRE(hvl == fnv1a::hash(&rdv, sizeof(rdv)));
	}
}