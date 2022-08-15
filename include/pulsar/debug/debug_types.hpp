/*! @file   debug_types.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Implements debug types.
 *  @date   11-08-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_DEBUG_FLAGS_HPP
#define PULSAR_DEBUG_FLAGS_HPP 1

// Include: Pulsar -> Debug
#include "pulsar/pulsar.hpp"

// Include: C++
#include <string>
#include <vector>

// Pulsar
namespace pul
{
	/*! @brief
	 *
	 */
	namespace debug_flags
	{
		pf_decl_constexpr uint32_t none					 = 0x00000000;
		pf_decl_constexpr uint32_t write_in_logs = 0x00000001; // Write in logs when exception is called.

		// ! Dumpfile is only generated in release mode !
		pf_decl_constexpr uint32_t generate_dumpfile											= 0x00000002;											// Generate dump file with default data.
		pf_decl_constexpr uint32_t dump_with_data_segs										= generate_dumpfile | 0x00000004; // Dump with data sections of all modules (global variables).
		pf_decl_constexpr uint32_t dump_with_full_memory									= generate_dumpfile | 0x00000008; // Dump with all accessible memory of the process.
		pf_decl_constexpr uint32_t dump_with_handle_data									= generate_dumpfile | 0x00000010; // Dump with high-level information of the OS.
		pf_decl_constexpr uint32_t dump_with_filter_memory								= generate_dumpfile | 0x00000020; // Dump with stack and backing store memory filtered.
		pf_decl_constexpr uint32_t dump_with_scan_memory									= generate_dumpfile | 0x00000040; // Dump with stack and backing store memory scanned for reference on modules.
		pf_decl_constexpr uint32_t dump_with_unloaded_modules							= generate_dumpfile | 0x00000080; // Dump with unloaded modules information.
		pf_decl_constexpr uint32_t dump_with_indirectly_referenced_memory = generate_dumpfile | 0x00000100; // Dump with pages referenced by locals or other stack memory.
		pf_decl_constexpr uint32_t dump_with_filter_module_paths					= generate_dumpfile | 0x00000200; // Dump with module path filtering such as user names or important directories.
		pf_decl_constexpr uint32_t dump_with_process_thread_data					= generate_dumpfile | 0x00000400; // Dump with complete per-process and per-thread information from OS.
		pf_decl_constexpr uint32_t dump_with_private_readwrite_memory			= generate_dumpfile | 0x00000800; // Dump with scan of virtual address space with read and write access.
		pf_decl_constexpr uint32_t dump_without_auxiliary_state						= generate_dumpfile | 0x00001000; // Dump without auxiliary-supported memory gathering.
		pf_decl_constexpr uint32_t dump_with_full_auxiliary_state					= generate_dumpfile | 0x00002000; // Dump with full auxiliary data.
		pf_decl_constexpr uint32_t dump_with_private_write								= generate_dumpfile | 0x00004000; // Dump with virtual address with write only access.
		pf_decl_constexpr uint32_t dump_without_inaccessible_memory				= generate_dumpfile | 0x00008000; // Dump without inaccessible memory.
		pf_decl_constexpr uint32_t dump_with_token_information						= generate_dumpfile | 0x00010000; // Dump with security tokens for user data.
		pf_decl_constexpr uint32_t dump_with_module_headers								= generate_dumpfile | 0x00020000; // Dump with module header related data.
		pf_decl_constexpr uint32_t dump_with_filter_triage								= generate_dumpfile | 0x00040000; // Dump with filter triage related data.
		pf_decl_constexpr uint32_t dump_with_avx_state_context						= generate_dumpfile | 0x00080000; // Dump with avx state context.
		pf_decl_constexpr uint32_t dump_with_ipt_trace										= generate_dumpfile | 0x00100000; // Dump with Intel processor trace related data.
		pf_decl_constexpr uint32_t dump_with_inaccessible_partial_pages		= generate_dumpfile | 0x00200000; // Dump with inaccessible partial pages.
		pf_decl_constexpr uint32_t dump_with_valid_type_flags							= generate_dumpfile | 0x00400000; // Dump with valid type flags.
		// ! Dumpfile is only generated in release mode !

		pf_decl_constexpr uint32_t default_flags = write_in_logs
																						 | generate_dumpfile;
	}

	/// Debug Trace
	/*! @brief Structure containing the trace of a symbol.
	 */
	struct debug_trace_t
	{
		const std::string undname;
		const std::string name;
		const std::string filename;
		const std::string modulename;
		const uint32_t fileline;
	};

	/// Debug Trace -> List
	using debug_trace_list_t = std::vector<debug_trace_t>;

	/// Levels
	/*! @brief Structure listing the levels of the messages.
	 */
	enum class debug_level : char
	{
		info		= 'I',
		warning = 'W',
		error		= 'E'
	};

	/// Filter
	/*! @brief Structure listing the message filters.
	 */
	enum class debug_filter : byte_t
	{
		important,
		helpful,
		detail,
	};
}

#endif // !PULSAR_DEBUG_FLAGS_HPP