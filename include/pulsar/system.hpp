/*! @file   system.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Introduces functions allowing to manage the system.
 *  @date   15-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

// Include: Pulsar
#include "pulsar/mathematics.hpp"
#include "pulsar/pulsar.hpp"

// Include: C++
#include <string>
#include <string_view>

// Pulsar
namespace pul
{
	/// SYSTEM: CPU
	/*! @brief Structure containing the information of the processor attached to this process.
	 */
	struct cpu_info_t
	{
		size_t nNUMA;
		size_t nLogical;
		size_t nPhysical;
		size_t nPhysicalPackages;
		size_t sL1;
		size_t lL1;
		size_t sL2;
		size_t lL2;
		size_t sL3;
		size_t lL3;
	};

	/*! @brief
	 *
	 *  @return pulsar_api
	 */
	pf_hint_nodiscard pulsar_api cpu_info_t cpu_info();

	/*! @brief Get the name of the processor attached to this process.
	 *
	 *  @return std::string containing the name of the processor.
	 */
	pulsar_api pf_hint_nodiscard std::string cpu_name() pf_attr_noexcept;

	/*! @brief Get the vendor name of the processor attached to this process.
	 *
	 *  @return std::string containing the vendor name of the processor.
	 */
	pulsar_api pf_hint_nodiscard std::string cpu_vendor() pf_attr_noexcept;

	/*! @brief Check if the processor is from the Intel brand.
	 *
	 *  @param __vendor Vendor's name.
	 *  @return True if is from Intel.
	 *  @return False if not.
	 */
	pf_hint_nodiscard pf_decl_inline bool cpu_is_intel(
			std::string_view __vendor) pf_attr_noexcept
	{
		return __vendor == "GenuineIntel";
	}
	/*! @brief Check if the processor is from the Intel brand.
	 *
	 *  @return True if is from Intel.
	 *  @return False if not.
	 */
	pf_hint_nodiscard pf_decl_inline bool cpu_is_intel() pf_attr_noexcept
	{
		return cpu_is_intel(cpu_vendor());
	}

	/*! @brief Check if the processor is from the AMD brand.
	 *
	 *  @param __vendor Vendor's name.
	 *  @return True if is from AMD.
	 *  @return False if not.
	 */
	pf_hint_nodiscard pf_decl_inline bool cpu_is_amd(
			std::string_view __vendor) pf_attr_noexcept
	{
		return __vendor == "AuthenticAMD";
	}
	/*! @brief Check if the processor is from the AMD brand.
	 *
	 *  @return True if is from AMD.
	 *  @return False if not.
	 */
	pf_hint_nodiscard pf_decl_inline bool cpu_is_amd() pf_attr_noexcept
	{
		return cpu_is_amd(cpu_vendor());
	}

	/*! @brief Check if hyperthreading is enabled on this processor.
	 *
	 *  @param __info Information structure of the processor.
	 *  @return True if enabled.
	 *  @return Else if not.
	 */
	pf_hint_nodiscard pf_decl_inline bool cpu_hyperthreading_enabled(
			cpu_info_t const &__info) pf_attr_noexcept
	{
		return __info.nLogical == 2 * __info.nPhysical;
	}
	/*! @brief Check if hyperthreading is enabled on this processor.
	 *
	 *  @return True if enabled.
	 *  @return Else if not.
	 */
	pf_hint_nodiscard pf_decl_inline bool cpu_hyperthreading_enabled() pf_attr_noexcept
	{
		return cpu_hyperthreading_enabled(cpu_info());
	}

	/// SYSTEM: Instruction Sets
	/// f_1_ECX
	pulsar_api pf_hint_nodiscard bool cpu_sse3() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_pclmulqdq() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_monitor() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_ssse3() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_fma() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_cmpxchg16b() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_sse41() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_sse42() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_movbe() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_popcnt() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_aes() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_xsave() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_osxsave() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_avx() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_f16c() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_rdrand() pf_attr_noexcept;

	/// f_1_EDX
	pulsar_api pf_hint_nodiscard bool cpu_msr() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_cx8() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_sep() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_cmov() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_clfsh() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_mmx() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_fxsr() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_sse() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_sse2() pf_attr_noexcept;

	/// f_7_EBX
	pulsar_api pf_hint_nodiscard bool cpu_fsgsbase() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_bmi1() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_hle() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_avx2() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_bmi2() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_erms() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_invpcid() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_rtm() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_avx512f() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_rdseed() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_adx() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_avx512pf() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_avx512er() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_avx512cd() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_sha() pf_attr_noexcept;

	/// f_7_ECX
	pulsar_api pf_hint_nodiscard bool cpu_prefetchwt1() pf_attr_noexcept;

	/// f_81_ECX
	pulsar_api pf_hint_nodiscard bool cpu_lahf() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_lzcnt() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_abm() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_sse4a() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_xop() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_tbm() pf_attr_noexcept;

	/// f_81_EDX
	pulsar_api pf_hint_nodiscard bool cpu_syscall() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_mmxext() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_rdtscp() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_mmx3Dnowext() pf_attr_noexcept;
	pulsar_api pf_hint_nodiscard bool cpu_mmx3Dnow() pf_attr_noexcept;

	/// SYSTEM: RAM
	/*! @brief Structure containing information about memory usage by this process.
	 */
	struct ram_process_info_t
	{
		size_t sVirtUsage;		 // Total virtual memory usage size.
		size_t sPhysUsage;		 // Total physical memory usage size.
		size_t sPageFileUsage; // Total pagefile usage size.
		size_t nPageFileFault; // Number of page faults.
	};
	/*! @brief Get the memory usage of this process when this function is called.
	 *
	 *  @return Structure containing memory usage information by this process.
	 */
	pf_hint_nodiscard pulsar_api ram_process_info_t ram_process_info();

	/*! @brief Structure containing information about memory status of the system.
	 */
	struct ram_system_info_t
	{
		size_t sVirtTotal;		 // Total virtual memory size.
		size_t sVirtAvail;		 // Available virtual memory size.
		size_t sVirtAvailExt;	 // Available extended virtual memory size.
		size_t sPhysTotal;		 // Total physical memory size.
		size_t sPhysAvail;		 // Available physical memory size.
		size_t sPageFileTotal; // Total pagefile memory size.
		size_t sPageFileAvail; // Available pagefile memory size.
	};
	/*! @brief Get the memory status of this system when this function is called.
	 *
	 *  @return Structure containing memory status information by this system.
	 */
	pf_hint_nodiscard pulsar_api ram_system_info_t ram_system_info();

	/// VIRTUAL: Access
	pf_decl_constexpr uint32_t VIRTUAL_ACC_NOACCESS_BIT = 0x0000u;
	pf_decl_constexpr uint32_t VIRTUAL_ACC_READ_BIT			= 0x0001u;
	pf_decl_constexpr uint32_t VIRTUAL_ACC_WRITE_BIT		= 0x0002u;
	pf_decl_constexpr uint32_t VIRTUAL_ACC_EXECUTE_BIT	= 0x0004u;
	pf_decl_constexpr uint32_t VIRTUAL_ACC_GUARD_BIT		= 0x0008u;
	pf_decl_constexpr uint32_t VIRTUAL_ACC_NOCACHE_BIT	= 0x0010u;

	/// VIRTUAL: Functions
	/*! @brief Reserve a memory page of size @a __size.
	 *
	 *  @param[in] __size Size in bytes of the reserved page.
	 *  @return Base address of the reserved page.
	 */
	pulsar_api pf_hint_nodiscard void *virtual_reserve(
			size_t __size);
	/*! @brief Commit memory of a reserved page file at address @a __where of size @a __size in
	 *				 bytes and grant memory accesses according to @a __access flags.
	 *
	 *  @param[in] __where  Address of reserved memory such as @a __where + @a __size < __base + page
	 *  									  reserved size in bytes.
	 *  @param[in] __size		Size in bytes of the memory to reserved.
	 *  @param[in] __access Memory access flags to be granted.
	 *  @return Base address of the page file.
	 */
	pulsar_api void *virtual_alloc(
			void *__where,
			size_t __size,
			uint32_t __access);
	/*! @brief Decommit memory from a reserved page file at address @a __where of size @a __size in
	 *				 bytes.
	 *
	 *  @param[in] __where Address of reserved memory such as @a __where + @a __size < __base + page
	 *  									 reserved size in bytes.
	 *  @param[in] __size	 If __size == 0, decommit all the page file.
	 * 										 Otherwise, decommit __where to __where + __size range of the page file.
	 */
	pulsar_api void virtual_free(
			void *__where,
			size_t __size);
	/*! @brief Unreserve an entire page file starting with address @a __base.
	 *
	 *  @param[in] __base The base address of the page file.
	 */
	pulsar_api void virtual_release(
			void *__base);
	/*! @brief Change memory access permissions of a committed region of a page file.
	 *
	 *  @param[in] __where  Address of comitted memory.
	 *  @param[in] __size   Size in bytes of the comitted memory region such as __where + __size is a
	 * 										  comitted memory region sequence.
	 *  @param[in] __access New access flags.
	 *  @return Old access flags.
	 */
	pulsar_api pf_hint_nodiscard uint32_t virtual_access(
			void *__where,
			size_t __size,
			uint32_t __access);


	/// SYSTEM: Info
	/*! @brief Structure containing the essential system information.
	 */
	struct system_info_t
	{
		size_t pageSize;
		size_t numProcessors;
		size_t allocationGranularity;
		void *minAllocationAddress;
		void *maxAllocationAddress;
	};

	/*! @brief Returns essential system information.
	 *
	 *  @return Essential system information.
	 */
	pulsar_api pf_hint_nodiscard system_info_t system_info() pf_attr_noexcept;
}