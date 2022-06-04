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
	TEST_CASE("SystemCPUInfoTest")
	{
		cpu_info_t info = cpu_info();
		REQUIRE(info.nPhysical > 0);
		std::string name = cpu_name();
		REQUIRE(!name.empty());
		std::string vendor = cpu_vendor();
		REQUIRE(!vendor.empty());
	}

	/// RAM: Test Unit
	TEST_CASE("SystemMemoryRAMInfoTest")
	{
		auto pi = ram_process_info();
		REQUIRE(pi.sPhysUsage > 0);
		auto si = ram_system_info();
		REQUIRE(si.sPhysTotal > 0);
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
		const uint32_t access = VIRTUAL_ACC_WRITE_BIT
													| VIRTUAL_ACC_READ_BIT
													| VIRTUAL_ACC_EXECUTE_BIT;
		as_void = virtual_alloc(
				b,
				sysinfo.pageSize,
				access);
		REQUIRE(as_void == b);
		uint32_t f = virtual_access(
				as_void,
				sysinfo.pageSize,
				VIRTUAL_ACC_WRITE_BIT | VIRTUAL_ACC_READ_BIT);
		REQUIRE(f == access);
		virtual_free(as_void, sysinfo.pageSize);
		virtual_release(b);
	}
}