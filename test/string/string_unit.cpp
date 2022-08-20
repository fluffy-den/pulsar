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
	// String Format
	TEST_CASE("StringFormatTest")
	{
		std::format const char *fmt = "%s";
		const char *msg							= "test";
		std::string str							= strfmt(fmt, msg);
		REQUIRE(str == "test");
		REQUIRE(str.back() != '\0');
	}
	TEST_CASE("StringFormatRefTest")
	{
		const char *fmt = " %s";
		const char *msg = "test";
		std::string str = "str with";
		strfmt(fmt, str, str.end(), msg);
		REQUIRE(str == "str with test");
		REQUIRE(str.back() != '\0');
	}
	TEST_CASE("StringTimeFormatTest")
	{
		const char *fmt = "%c";
		std::time_t t		= std::time(0);
		std::tm *k			= std::localtime(&t);
		std::string str = strfmt(fmt, *k);
		REQUIRE(!str.empty());
		REQUIRE(str.back() != '\0');
	}
	TEST_CASE("StringTimeFormatRefTest")
	{
		const char *fmt = " %c";
		std::string str = "str date";
		std::time_t t		= std::time(0);
		std::tm *k			= std::localtime(&t);
		strfmt(fmt, str, str.end(), *k);
		REQUIRE(str.length() != 8);
		REQUIRE(str.back() != '\0');
	}

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