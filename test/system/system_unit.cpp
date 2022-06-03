/*! @file   system_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Test unit for system operating functions.
 *  @date   15-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

// Include: Pulsar
#include "pulsar/system.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Pulsar
namespace pul
{
	/// CPU: Test Unit
	TEST_CASE("CPUInfoTest")
	{
		cpu_info_t info = cpu_info();
		std::ignore			= info;
	}
	TEST_CASE("CPUNameTest")
	{
		std::string name = cpu_name();
		REQUIRE(!name.empty());
	}
	TEST_CASE("CPUVendorTest")
	{
		std::string vendor = cpu_vendor();
		REQUIRE(!vendor.empty());
	}

	/// RAM: Test Unit
	TEST_CASE("MemoryRAMProcessInfoTest")
	{
		auto info		= ram_process_info();
		std::ignore = info;
	}
	TEST_CASE("MemoryRAMSystemInfoTest")
	{
		auto info		= ram_system_info();
		std::ignore = info;
	}
}