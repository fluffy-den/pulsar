/*! @file   string_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   16-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

// Include: Pulsar
#include "pulsar/string.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Pulsar
namespace pul
{
	// String Utilities
	TEST_CASE("StringTrimLeftTest")
	{
		std::string str = "			message";
		strtriml(str);
		REQUIRE(str == "message");
	}
	TEST_CASE("StringTrimRightTest")
	{
		std::string str = "message			";
		strtrimr(str);
		REQUIRE(str == "message");
	}
	TEST_CASE("StringToLowerTest")
	{
		std::string str = "MeSSaGE";
		strtolower(str);
		REQUIRE(str == "message");
	}
	TEST_CASE("StringToUpperTest")
	{
		std::string str = "MeSSaGE";
		strtoupper(str);
		REQUIRE(str == "MESSAGE");
	}
}