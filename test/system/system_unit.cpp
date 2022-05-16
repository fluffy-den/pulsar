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
#include "pulsar/memory.hpp"

// Include: Catch2
#include "catch2/catch_all.hpp"

// Pulsar
namespace pul
{
	/// RAM: Test
	TEST_CASE("MemoryRAMProcessInfoTest")
	{
		auto info		= memory::ram_process_info();
		std::ignore = info;
	}
	TEST_CASE("MemoryRAMSystemInfoTest")
	{
		auto info		= memory::ram_system_info();
		std::ignore = info;
	}
}