/*! @file   ram_win.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  RAM information implementation under windows.
 *  @date   15-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

// Include: Pulsar
#include "pulsar/debug.hpp"
#include "pulsar/pulsar.hpp"
#include "pulsar/system.hpp"

// Include: C
#include <cstring>

// Include: Windows
#ifdef PF_OS_WINDOWS
#	include <windows.h>
#	include <psapi.h>

// Pulsar
namespace pul
{
	/// RAM: Info
	pf_hint_nodiscard pulsar_api ram_process_info_t ram_process_info()
	{
		PROCESS_MEMORY_COUNTERS_EX pmce;
		std::memset(&pmce, 0, sizeof(pmce));
		if (!GetProcessMemoryInfo(
						GetCurrentProcess(),
						reinterpret_cast<PROCESS_MEMORY_COUNTERS *>(&pmce),
						sizeof(pmce)))
			throw(exception(
					std::system_category(),
					GetLastError()));
		ram_process_info_t info;
		info.sVirtUsage			= pmce.WorkingSetSize;
		info.sPhysUsage			= pmce.PagefileUsage;
		info.sPageFileUsage = pmce.PeakWorkingSetSize;
		info.nPageFileFault = pmce.PageFaultCount;
		return info;
	}
	pf_hint_nodiscard pulsar_api ram_system_info_t ram_system_info()
	{
		MEMORYSTATUSEX msex;
		std::memset(&msex, 0, sizeof(msex));
		msex.dwLength = sizeof(msex);
		if (!GlobalMemoryStatusEx(&msex))
			throw(exception(
					std::system_category(),
					GetLastError()));
		ram_system_info_t info;
		info.sVirtTotal			= msex.ullTotalVirtual;
		info.sVirtAvail			= msex.ullAvailVirtual;
		info.sVirtAvailExt	= msex.ullAvailExtendedVirtual;
		info.sPhysTotal			= msex.ullTotalPhys;
		info.sPhysAvail			= msex.ullAvailPhys;
		info.sPageFileTotal = msex.ullTotalPageFile;
		info.sPageFileAvail = msex.ullAvailPageFile;
		return info;
	}
}

#endif // PF_OS_WINDOWS