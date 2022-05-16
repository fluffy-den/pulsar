/*! @file   cpu_unit.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Test unit for CPU.
 *  @date   20-03-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

// Include: Pulsar
#include "pulsar/cpu.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Pulsar
namespace pul
{

	// CPU
	TEST_CASE("CPUInfoTest")
	{
		cpu_info_t info = cpu_info();
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
}