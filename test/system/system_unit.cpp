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

	/// VIRTUAL: Test Unit
	TEST_CASE("VirtualMemoryTest")
	{
		system_info_t sysinfo = system_info();
		REQUIRE(sysinfo.allocationGranularity >= 65536);
		REQUIRE(sysinfo.pageSize >= 4096);
		void *b = virtual_reserve(sysinfo.allocationGranularity);
		REQUIRE(b != nullptr);
		union
		{
			void *as_void;
			int32_t *as_int;
		};
		void *a = virtual_alloc(b, sysinfo.pageSize, VIRTUAL_ACC_WRITE_BIT);
		REQUIRE(a == b);
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int32_t> r(
				std::numeric_limits<int32_t>::min(),
				std::numeric_limits<int32_t>::max());
		int32_t n = r(gen);
		std::memset(a, n, sysinfo.allocationGranularity);
		uint32_t f = virtual_access(a, sysinfo.pageSize, VIRTUAL_ACC_WRITE_BIT | VIRTUAL_ACC_READ_BIT);
		REQUIRE(f == VIRTUAL_ACC_WRITE_BIT);
		REQUIRE(*as_int == n);
		virtual_free(a, sysinfo.pageSize);
		virtual_release(b);
	}
}