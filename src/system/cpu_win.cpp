/*! @file   cpu_win.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Implements the CPU utilities under Windows.
 *  @date   20-03-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

// Include: Pulsar
#include "pulsar/debug.hpp"
#include "pulsar/system.hpp"

// Include: Windows
#ifdef PF_OS_WINDOWS
#	include <windows.h>
#	include <intrin.h>
#	include <sysinfoapi.h>

// Include: C++
#	include <array>
#	include <memory>

// Include: C
#	include <cstring>

// Pulsar
namespace pul
{

	/// CPU: Info
	pulsar_api cpu_info_t cpu_info()
	{
		DWORD len;
		cpu_info_t info;
		GetLogicalProcessorInformation(nullptr, &len);
		std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> store(
				len / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
		if (!GetLogicalProcessorInformation(store.data(), &len))
			throw(exception(
					std::system_category(),
					GetLastError(),
					"Couldn't retrieve CPU information!"));
		for (auto &it : store)
		{
			switch (it.Relationship)
			{
				case RelationNumaNode:
				{
					++info.nNUMA;
					break;
				}
				case RelationProcessorCore:
				{
					++info.nPhysical;
					DWORD lShift			 = sizeof(ULONG_PTR) * 8 - 1;
					ULONG_PTR lBitTest = static_cast<ULONG_PTR>(1) << lShift;
					for (DWORD i = 0; i <= lShift; ++i)
					{
						info.nLogical += ((it.ProcessorMask & lBitTest) ? 1 : 0);
						lBitTest /= 2;
					}
					break;
				}
				case RelationCache:
				{
					switch (it.Cache.Level)
					{
						case 1:
							info.sL1 += it.Cache.Size;
							info.lL1 += it.Cache.LineSize;
							break;
						case 2:
							info.sL2 += it.Cache.Size;
							info.lL2 += it.Cache.LineSize;
							break;
						case 3:
							info.sL3 += it.Cache.Size;
							info.lL3 += it.Cache.LineSize;
							break;
					}
					break;
				}
				case RelationProcessorPackage:
				{
					++info.nPhysicalPackages;
					break;
				}
				default:
				{
					break;
				}
			}
		}
		return info;
	}

	/// CPU: Name
	pulsar_api pf_hint_nodiscard std::string cpu_name() pf_attr_noexcept
	{
		std::array<int32_t, 4> cpui;
		__cpuid(cpui.data(), 0x80000000);
		uint32_t nexids = static_cast<uint32_t>(cpui[0]);
		if (nexids >= 0x80000004)
		{
			std::vector<std::array<int32_t, 4>> store;
			store.reserve(nexids - 0x80000002);
			for (uint32_t i = 0x80000002; i < nexids; ++i)
			{
				__cpuidex(cpui.data(), i, 0);
				store.push_back(cpui);
			}
			char name[0x40]{ ' ' };
			std::memcpy(&name[00], store[0].data(), sizeof(cpui));
			std::memcpy(&name[16], store[1].data(), sizeof(cpui));
			std::memcpy(&name[32], store[2].data(), sizeof(cpui));
			return name;
		}
		return "";
	}

	/// CPU: Vendor
	pulsar_api pf_hint_nodiscard std::string cpu_vendor() pf_attr_noexcept
	{
		std::array<int32_t, 4> cpui;
		__cpuidex(cpui.data(), 0, 0);
		char vendor[0x20]{ ' ' };
		*reinterpret_cast<int32_t *>(&vendor[0]) = cpui[1];
		*reinterpret_cast<int32_t *>(&vendor[4]) = cpui[3];
		*reinterpret_cast<int32_t *>(&vendor[8]) = cpui[2];
		return vendor;
	}

	/// CPU: Instruction Set
	bool __instruction_set(
			int32_t __ID,
			uint32_t __index,
			uint32_t __offset)
	{
		std::array<int32_t, 4> cpui;
		__cpuid(cpui.data(), 0);
		int32_t nids = cpui[0];
		if (nids >= __ID)
		{
			__cpuidex(cpui.data(), __ID, 0);
			return (cpui[__index] >> __offset) & 0b1;
		}
		return false;
	}
	bool __instruction_set_extended(
			int32_t __exID,
			uint32_t __index,
			uint32_t __offset)
	{
		__exID += 0x80000000;
		std::array<int32_t, 4> cpui;
		__cpuid(cpui.data(), 0x80000000);
		int32_t nexids = cpui[0];
		if (nexids >= __exID)
		{
			__cpuidex(cpui.data(), __exID, 0);
			return (cpui[__index] >> __offset) & 0b1;
		}
		return false;
	}

	/// f_1_ECX
	pulsar_api pf_hint_nodiscard bool cpu_sse3() pf_attr_noexcept
	{
		return __instruction_set(1, 2, 0);
	}
	pulsar_api pf_hint_nodiscard bool cpu_pclmulqdq() pf_attr_noexcept
	{
		return __instruction_set(1, 2, 1);
	}
	pulsar_api pf_hint_nodiscard bool cpu_monitor() pf_attr_noexcept
	{
		return __instruction_set(1, 2, 0);
	}
	pulsar_api pf_hint_nodiscard bool cpu_ssse3() pf_attr_noexcept
	{
		return __instruction_set(1, 2, 9);
	}
	pulsar_api pf_hint_nodiscard bool cpu_fma() pf_attr_noexcept
	{
		return __instruction_set(1, 2, 12);
	}
	pulsar_api pf_hint_nodiscard bool cpu_cmpxchg16b() pf_attr_noexcept
	{
		return __instruction_set(1, 2, 13);
	}
	pulsar_api pf_hint_nodiscard bool cpu_sse41() pf_attr_noexcept
	{
		return __instruction_set(1, 2, 19);
	}
	pulsar_api pf_hint_nodiscard bool cpu_sse42() pf_attr_noexcept
	{
		return __instruction_set(1, 2, 20);
	}
	pulsar_api pf_hint_nodiscard bool cpu_movbe() pf_attr_noexcept
	{
		return __instruction_set(1, 2, 22);
	}
	pulsar_api pf_hint_nodiscard bool cpu_popcnt() pf_attr_noexcept
	{
		return __instruction_set(1, 2, 23);
	}
	pulsar_api pf_hint_nodiscard bool cpu_aes() pf_attr_noexcept
	{
		return __instruction_set(1, 2, 25);
	}
	pulsar_api pf_hint_nodiscard bool cpu_xsave() pf_attr_noexcept
	{
		return __instruction_set(1, 2, 26);
	}
	pulsar_api pf_hint_nodiscard bool cpu_osxsave() pf_attr_noexcept
	{
		return __instruction_set(1, 2, 27);
	}
	pulsar_api pf_hint_nodiscard bool cpu_avx() pf_attr_noexcept
	{
		return __instruction_set(1, 2, 28);
	}
	pulsar_api pf_hint_nodiscard bool cpu_f16c() pf_attr_noexcept
	{
		return __instruction_set(1, 2, 29);
	}
	pulsar_api pf_hint_nodiscard bool cpu_rdrand() pf_attr_noexcept
	{
		return __instruction_set(1, 2, 30);
	}

	/// f_1_EDX
	pulsar_api pf_hint_nodiscard bool cpu_msr() pf_attr_noexcept
	{
		return __instruction_set(1, 3, 5);
	}
	pulsar_api pf_hint_nodiscard bool cpu_cx8() pf_attr_noexcept
	{
		return __instruction_set(1, 3, 8);
	}
	pulsar_api pf_hint_nodiscard bool cpu_sep() pf_attr_noexcept
	{
		return __instruction_set(1, 3, 11);
	}
	pulsar_api pf_hint_nodiscard bool cpu_cmov() pf_attr_noexcept
	{
		return __instruction_set(1, 3, 15);
	}
	pulsar_api pf_hint_nodiscard bool cpu_clfsh() pf_attr_noexcept
	{
		return __instruction_set(1, 3, 19);
	}
	pulsar_api pf_hint_nodiscard bool cpu_mmx() pf_attr_noexcept
	{
		return __instruction_set(1, 3, 23);
	}
	pulsar_api pf_hint_nodiscard bool cpu_fxsr() pf_attr_noexcept
	{
		return __instruction_set(1, 3, 24);
	}
	pulsar_api pf_hint_nodiscard bool cpu_sse() pf_attr_noexcept
	{
		return __instruction_set(1, 3, 25);
	}
	pulsar_api pf_hint_nodiscard bool cpu_sse2() pf_attr_noexcept
	{
		return __instruction_set(1, 3, 26);
	}

	/// f_7_EBX
	pulsar_api pf_hint_nodiscard bool cpu_fsgsbase() pf_attr_noexcept
	{
		return __instruction_set(7, 1, 0);
	}
	pulsar_api pf_hint_nodiscard bool cpu_bmi1() pf_attr_noexcept
	{
		return __instruction_set(7, 1, 3);
	}
	pulsar_api pf_hint_nodiscard bool cpu_hle() pf_attr_noexcept
	{
		return __instruction_set(7, 1, 4) && cpu_is_intel();
	}
	pulsar_api pf_hint_nodiscard bool cpu_avx2() pf_attr_noexcept
	{
		return __instruction_set(7, 1, 5);
	}
	pulsar_api pf_hint_nodiscard bool cpu_bmi2() pf_attr_noexcept
	{
		return __instruction_set(7, 1, 8);
	}
	pulsar_api pf_hint_nodiscard bool cpu_erms() pf_attr_noexcept
	{
		return __instruction_set(7, 1, 9);
	}
	pulsar_api pf_hint_nodiscard bool cpu_invpcid() pf_attr_noexcept
	{
		return __instruction_set(7, 1, 10);
	}
	pulsar_api pf_hint_nodiscard bool cpu_rtm() pf_attr_noexcept
	{
		return __instruction_set(7, 1, 11) && cpu_is_intel();
	}
	pulsar_api pf_hint_nodiscard bool cpu_avx512f() pf_attr_noexcept
	{
		return __instruction_set(7, 1, 16);
	}
	pulsar_api pf_hint_nodiscard bool cpu_rdseed() pf_attr_noexcept
	{
		return __instruction_set(7, 1, 18);
	}
	pulsar_api pf_hint_nodiscard bool cpu_adx() pf_attr_noexcept
	{
		return __instruction_set(7, 1, 19);
	}
	pulsar_api pf_hint_nodiscard bool cpu_avx512pf() pf_attr_noexcept
	{
		return __instruction_set(7, 1, 26);
	}
	pulsar_api pf_hint_nodiscard bool cpu_avx512er() pf_attr_noexcept
	{
		return __instruction_set(7, 1, 27);
	}
	pulsar_api pf_hint_nodiscard bool cpu_avx512cd() pf_attr_noexcept
	{
		return __instruction_set(7, 1, 28);
	}
	pulsar_api pf_hint_nodiscard bool cpu_sha() pf_attr_noexcept
	{
		return __instruction_set(7, 1, 29);
	}

	/// f_7_ECX
	pulsar_api pf_hint_nodiscard bool cpu_prefetchwt1() pf_attr_noexcept
	{
		return __instruction_set(7, 2, 0);
	}

	/// f_81_ECX
	pulsar_api pf_hint_nodiscard bool cpu_lahf() pf_attr_noexcept
	{
		return __instruction_set_extended(1, 2, 0);
	}
	pulsar_api pf_hint_nodiscard bool cpu_lzcnt() pf_attr_noexcept
	{
		return __instruction_set_extended(1, 2, 5) && cpu_is_intel();
	}
	pulsar_api pf_hint_nodiscard bool cpu_abm() pf_attr_noexcept
	{
		return __instruction_set_extended(1, 2, 5) && cpu_is_amd();
	}
	pulsar_api pf_hint_nodiscard bool cpu_sse4a() pf_attr_noexcept
	{
		return __instruction_set_extended(1, 2, 6) && cpu_is_amd();
	}
	pulsar_api pf_hint_nodiscard bool cpu_xop() pf_attr_noexcept
	{
		return __instruction_set_extended(1, 2, 11) && cpu_is_amd();
	}
	pulsar_api pf_hint_nodiscard bool cpu_tbm() pf_attr_noexcept
	{
		return __instruction_set_extended(1, 2, 21) && cpu_is_amd();
	}

	/// f_81_EDX
	pulsar_api pf_hint_nodiscard bool cpu_syscall() pf_attr_noexcept
	{
		return __instruction_set_extended(1, 3, 11) && cpu_is_intel();
	}
	pulsar_api pf_hint_nodiscard bool cpu_mmxext() pf_attr_noexcept
	{
		return __instruction_set_extended(1, 3, 22) && cpu_is_amd();
	}
	pulsar_api pf_hint_nodiscard bool cpu_rdtscp() pf_attr_noexcept
	{
		return __instruction_set_extended(1, 3, 27) && cpu_is_intel();
	}
	pulsar_api pf_hint_nodiscard bool cpu_mmx3Dnowext() pf_attr_noexcept
	{
		return __instruction_set_extended(1, 3, 30) && cpu_is_amd();
	}
	pulsar_api pf_hint_nodiscard bool cpu_mmx3Dnow() pf_attr_noexcept
	{
		return __instruction_set_extended(1, 3, 31) && cpu_is_amd();
	}
}

#endif // PF_OS_WINDOWS